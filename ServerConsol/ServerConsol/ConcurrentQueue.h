#pragma once
#include <mutex>

template<typename T>

class LockQueue {
public:
	LockQueue() {};

	LockQueue(const LockQueue&) = delete;
	LockQueue& operator=(const LockQueue&) = delete;

	void Push(T& value) {
		lock_guard<mutex> lock(_mutex);
		_queue.push(std::move(value));
		_condVar.notify_one();
	}


	bool TryPop(T& value) {
		lock_guard<mutex> lock(_mutex);

		if (_queue.empty())
			return false;

		value = std::move(_queue.front());
		_queue.pop();
		//외부에서 바로 pop()을 하여 가져온다면 복사과정에서 crash발생확률이있다.
		return true;
	}


	bool Empty() {
		lock_guard<mutex> lock(_mutex);
		return _queue.empty();
	}
	//멀티스레드 환경에서는 이런 검사가 크게 의미가 없다.(Empty와 Pop사이에 다른 스레드가 끼어들수 있음)

	void WaitPop(T& value) {
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this] {return _queue.empty() == false; });
		//조건을 만족할 때 까지 lock을 풀고 대기상태로 전환
		value = std::move(_queue.front());
		_queue.pop();

	}
private:
	mutex _mutex;
	queue<int32> _queue;
	condition_variable _condVar;
};

//
//template<typename T>
//class LockFreeQueue
//{
//	struct Node
//	{
//		shared_ptr<T> data;
//		Node* next = nullptr;
//	};
//
//public:
//	LockFreeQueue() : _head(new Node), _tail(_head)
//	{
//
//	}
//
//	LockFreeQueue(const LockFreeQueue&) = delete;
//	LockFreeQueue& operator=(const LockFreeQueue&) = delete;
//
//	void Push(const T& value)
//	{
//		shared_ptr<T> newData = make_shared<T>(value);
//
//		Node* dummy = new Node();
//
//		Node* oldTail = _tail;
//		oldTail->data.swap(newData);
//		oldTail->next = dummy;
//
//		_tail = dummy;
//	}
//
//	shared_ptr<T> TryPop()
//	{
//		Node* oldHead = PopHead();
//		if (oldHead == nullptr)
//			return shared_ptr<T>();
//
//		shared_ptr<T> res(oldHead->data);
//		delete oldHead;
//		return res;
//	}
//
//private:
//	Node* PopHead()
//	{
//		Node* oldHead = _head;
//		if (oldHead == _tail)
//			return nullptr;
//
//		_head = oldHead->next;
//		return oldHead;
//	}
//
//private:
//	// [data][data][]
//	// [head][tail]
//	Node* _head = nullptr;
//	Node* _tail = nullptr;
//};

template<typename T>
class LockFreeQueue
{
	struct Node;

	struct CountedNodePtr
	{
		int32 externalCount; // 참조권
		Node* ptr = nullptr;
	};

	struct NodeCounter
	{
		uint32 internalCount : 30; // 참조권 반환 관련
		uint32 externalCountRemaining : 2; // Push & Pop 다중 참조권 관련
	};

	struct Node
	{
		Node()
		{
			NodeCounter newCount;
			newCount.internalCount = 0;
			newCount.externalCountRemaining = 2;
			count.store(newCount);

			next.ptr = nullptr;
			next.externalCount = 0;
		}

		void ReleaseRef()
		{
			NodeCounter oldCounter = count.load();

			while (true)
			{
				NodeCounter newCounter = oldCounter;
				newCounter.internalCount--;

				// 끼어들 수 있음
				if (count.compare_exchange_strong(oldCounter, newCounter))
				{
					if (newCounter.internalCount == 0 && newCounter.externalCountRemaining == 0)
						delete this;

					break;
				}
			}
		}

		atomic<T*> data;
		atomic<NodeCounter> count;
		CountedNodePtr next;
	};

public:
	LockFreeQueue()
	{
		CountedNodePtr node;
		node.ptr = new Node;
		node.externalCount = 1;

		_head.store(node);
		_tail.store(node);
	}
	//락프리 큐 선언시 push동작 이전의 노드(dummy)를 생성해준다
	//head=tail=dummy
	LockFreeQueue(const LockFreeQueue&) = delete;
	LockFreeQueue& operator=(const LockFreeQueue&) = delete;

	// [data][]
	// [head][tail]
	void Push(const T& value)
	{
		//데이터 선언 : 순서 상관없는 부분
		unique_ptr<T> newData = make_unique<T>(value);

		CountedNodePtr dummy;
		dummy.ptr = new Node;
		dummy.externalCount = 1;

		CountedNodePtr oldTail = _tail.load(); // oldtail=tail=dummy
		
		//race condition 발생부
		while (true)
		{
			// 참조권 획득 (externalCount를 현시점 기준 +1 한 애가 이김)
			//참조권 획득 못할 시 함수 내에서 무한루프 진행
			IncreaseExternalCount(_tail, oldTail);
			

			// 소유권 획득 (data를 먼저를 교환한 애가 이김)
			T* oldData = nullptr;
			if (oldTail.ptr->data.compare_exchange_strong(oldData, newData.get()))
			{
				oldTail.ptr->next = dummy;
				oldTail = _tail.exchange(dummy);
				//oldtail의 tail에 대한 참조 메모리를 해제하기 위한 코드(권한 해제)
				// dummy가 가리키는 값과 _tail이 가리키는 값을 바꿔줌
				
				FreeExternalCount(oldTail);
				newData.release(); // 데이터에 대한 unique_ptr의 소유권 포기
				break;
			}

			// 소유권 경쟁 패배..
			oldTail.ptr->ReleaseRef();
		}
	}

	shared_ptr<T> TryPop()
	{
		// [data][data][ ]
		// [head][tail]

		CountedNodePtr oldHead = _head.load();

		while (true)
		{
			// 참조권 획득 (externalCount를 현시점 기준 +1 한 애가 이김)
			IncreaseExternalCount(_head, oldHead);

			Node* ptr = oldHead.ptr;
			if (ptr == _tail.load().ptr)
			{
				ptr->ReleaseRef();
				return shared_ptr<T>();
			}

			// 소유권 획득 (head = ptr->next)
			if (_head.compare_exchange_strong(oldHead, ptr->next))
			{
				T* res = ptr->data.load(); // exchange(nullptr); 로 하면 버그 있음!
				FreeExternalCount(oldHead);
				return shared_ptr<T>(res);
			}

			ptr->ReleaseRef();
		}
	}

private:
	static void IncreaseExternalCount(atomic<CountedNodePtr>& counter, CountedNodePtr& oldCounter)
	{
		while (true)
		{
			CountedNodePtr newCounter = oldCounter;
			newCounter.externalCount++;

			//if(tail==oldtail)  => tail=newCounter
			
			if (counter.compare_exchange_strong(oldCounter, newCounter))
			{
				//CAS가 성공하면 counter.externalCount는 증가한 값이 적용되어
				//다른 스레드에서는 해당 조건문이 무조건 실패하게 된다.
				oldCounter.externalCount = newCounter.externalCount;
				//oldCounter==oldtail의 externalCount를 증가시킨다
				break;
			}
		}
	}

	static void FreeExternalCount(CountedNodePtr& oldNodePtr)
	{
		Node* ptr = oldNodePtr.ptr;
		const int32 countIncrease = oldNodePtr.externalCount - 2;

		NodeCounter oldCounter = ptr->count.load();

		while (true)
		{
			NodeCounter newCounter = oldCounter;
			newCounter.externalCountRemaining--; // TODO
			newCounter.internalCount += countIncrease;

			if (ptr->count.compare_exchange_strong(oldCounter, newCounter))
			{
				if (newCounter.internalCount == 0 && newCounter.externalCountRemaining == 0)
					delete ptr;

				break;
			}
		}
	}

private:
	// [data][data][]
	// [head][tail]
	atomic<CountedNodePtr> _head;
	atomic<CountedNodePtr> _tail;
};