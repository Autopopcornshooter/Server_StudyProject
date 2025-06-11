#pragma once
#include <mutex>

template<typename T>

class LockStack {
public:
	LockStack() {}

	LockStack(const LockStack&) = delete;
	LockStack& operator=(const LockStack&) = delete;

	void Push(T value) {
		lock_guard<mutex> lock(_mutex);
		_stack.push(std::move(value));
		_condVar.notify_one();
	}
	bool TryPop(T& value) {
		lock_guard<mutex> lock(_mutex);

		if (_stack.empty())
			return false;
		
		value = std::move(_stack.top());
		_stack.pop();
		//외부에서 바로 pop()을 하여 가져온다면 복사과정에서 crash발생확률이있다.
		return true;
	}


	bool Empty() {
		lock_guard<mutex> lock(_mutex);
		return _stack.empty();
	}
	//멀티스레드 환경에서는 이런 검사가 크게 의미가 없다.(Empty와 Pop사이에 다른 스레드가 끼어들수 있음)

	void WaitPop(T& value) {
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this] {return _stack.empty() == false; });
		//조건을 만족할 때 까지 lock을 풀고 대기상태로 전환
		value = std::move(_stack.top());
		_stack.pop();

	}
private:

	stack<T>_stack;
	mutex _mutex;
	condition_variable _condVar;
};
//
//template<typename T>

//class LockFreeStack {
//
//	struct Node {
//		Node(const T& value): data(value), next(NULL) {
//		}
//
//		T data;
//		Node* next;
//	};
//
//public:
//
//	void Push(const T& value) {
//		Node* node = new Node(value);
//		node->next = _head;
//
//		while (node && _head.compare_exchange_strong(node->next, node) == false) {
//
//		}
//	}
//	bool TryPop(T& value) {
//		++_popCount;
//		Node* oldHead = _head;
//
//		while (oldHead && _head.compare_exchange_strong(oldHead, oldHead->next) == false) {
//
//		}
//		if (oldHead == nullptr) {
//			--_popCount;
//			return false;
//		}
//		value = oldHead->data;
//
//		TryDelete(oldHead);
//
//		return true;
//	}
//
//
//	//나만 TryPop을 쓰고 있는지 확인
//	// 데이터를 분리한 시점에서 검사하여 현재 스레드에서만 사용중이면 _pendinList의 노드 삭제
//	// 다른 스레드에서도 사용중이라면 분리한 노드 다시 돌려놓는다
//	//다른 스레드에서도 사용중이라면 pendingList에 삭제할 노드를 저장
//	void TryDelete(Node* oldHead) {
//		if (_popCount == 1) {
//			Node* node = _pendingList.exchange(nullptr);
//			if (--_popCount == 0) {
//				DeleteNodes(node);
//			}
//			else if(node) {
//				ChainPendingNodeList(node);
//			}
//			delete oldHead;
//		}
//		else {
//			ChainPendingNode(oldHead);
//		}
//	}
//
//	void DeleteNodes(Node* node) {
//		while (node) {
//			Node* next = node->next;
//			delete node;
//			node = next;
//		}
//
//	}
//	//_pendingList의 last노드에 first 노드를 연결한다(last->next=first)
//	void ChainPendingNodeList(Node* first, Node* last) {
//		last->next = _pendingList;
//		while (_pendingList.compare_exchange_strong(last->next, first) == false) {
//
//		}
//	}
//	//ChainPendingNodeList 헬퍼 함수
//	void ChainPendingNodeList(Node* node) {a
//		Node* last = node;
//		while (last->next) {
//			last = last->next;
//		}
//		ChainPendingNodeList(node, last);
//	}
//	//ChainPendingNodeList를 사용하여 하나의 노드를 _pendingList에 연결하는 함수
//	void ChainPendingNode(Node* node) {
//		ChainPendingNodeList(node, node);
//	}
//
//private:
//	atomic<Node*> _head;
//	
//	atomic<int32> _popCount = 0;
//
//	atomic<Node*> _pendingList;
//
//};

//class LockFreeStack {
//
//	struct Node {
//		Node(const T& value) : data(make_shared<T>(value)), next(nullptr) {}
//
//		shared_ptr<Node> next;	
//		shared_ptr<T> data;	
//	};
//
//public:
//	void Push(const T& value) {
//		shared_ptr<Node> node = make_shared<Node>(value);
//		node->next = atomic_load(&_head);
//		
//		while (atomic_compare_exchange_strong(&_head, &node->next, node) == false) {
//			//_head가 node->next와 같지 않으면 node->next를 _head로 바꾸고 node를 _head로 바꾼다
//			//_head가 node->next와 같으면 node를 _head로 바꾸지 않고 계속 반복한다
//		}
//	}
//
//	shared_ptr<T> TryPop() {
//		shared_ptr<Node> oldHead = atomic_load(&_head);
//		
//		while (oldHead && atomic_compare_exchange_weak(&_head, &oldHead, oldHead->next) == false) {
//			//_head가 oldHead와 같지 않으면 oldHead를 _head로 바꾸고 oldHead를 _head로 바꾼다
//			//_head가 oldHead와 같으면 oldHead를 _head로 바꾸지 않고 계속 반복한다
//		}
//
//		if (oldHead == nullptr) {
//			return shared_ptr<T>();
//		}
//		return oldHead->data; // oldHead의 data를 반환
//
//
//	}
//
//private:
//	shared_ptr<Node> _head;
//	// shared_ptr를 사용하여 노드의 메모리 관리를 자동으로 처리
//};

template<typename T>
class LockFreeStack {

	struct Node;

	struct CountedNodePtr {
		int32 externalCount = 0;	//헤드를 참조한 외부 스레드 수
		Node* ptr = nullptr;
	};

	struct Node {
		Node(const T& value) : data(make_shared<T>(value)) {
		}
		shared_ptr<T> data;
		atomic<int32> internalCount = 0;	//노드 내부에서의 참조 수
		CountedNodePtr next; 
	};

public:

	void Push(const T& value) {
		CountedNodePtr node;
		node.ptr = new Node(value);
		node.externalCount = 1;
		node.ptr->next = _head;	
		while (_head.compare_exchange_weak(node.ptr->next, node) == false) {
			
		}
	}

	shared_ptr<T> TryPop() {
		CountedNodePtr oldHead = _head;
		//_head를 참조하기 위한 권한을 획득하는 방식 사용
		while (true) {
			//참조권 획득 단계(externalCount를 현 시점 기준 +1한 스레드가 접근권한 획득)
			IncreaseHeadCount(oldHead);
			//최소한 externalCount >=2 일테니 삭제X(안전하게 접근할 수 있는 노드)
			Node* ptr = oldHead.ptr;
			
			
			//데이터가 없다면?
			if (ptr == nullptr) {
				return shared_ptr<T>();
			}

			//소유권 획득 단계 (ptr->next로 head를 바꿔치기 한 스레드가 이김)
			if (_head.compare_exchange_strong(oldHead, ptr->next)) {
				shared_ptr<T> res;
				res.swap(ptr->data);	//실제 자원의 복사나 이동 없이 참조만 교환

				//external 시작값 1 => 참조하려는 순간 +1(2)
				//internal 시작값 0 => 

				//나 말고 누가 또 있는가?
				const int32 countIncrease = oldHead.externalCount - 2;

				//fetch_add(x) x값만큼 더한뒤 더하기 전 값을 반환
				if (ptr->internalCount.fetch_add(countIncrease) == -countIncrease)
					delete ptr;

				return res;
			}
			else if(ptr->internalCount.fetch_sub(1)==1) {
				//fetch_add(x) x값만큼 뺀 뒤 빼기 전 값을 반환
				// 조건문이 성립한다면 참조권을 얻은 스레드는 하나뿐이다
				// 해당 스레드가 메모리 삭제를 수행한다.
				
				//참조권은 얻었으나 소유권은 획득 실패, 뒷수습은 내가 한다(?)
				delete ptr;
			}
		}
	}



private:
	void IncreaseHeadCount(CountedNodePtr& oldCounter) {
		while (true) {
			//동시접근 예상부분
			CountedNodePtr newCounter = oldCounter;	
			newCounter.externalCount++;	
			if (_head.compare_exchange_strong(oldCounter, newCounter)) {
				// 동시접근시 CAS를 먼저 수행한 스레드가 TryPop을 수행
				// 성공시 _head의 externalCount가 1증가
				//
				oldCounter.externalCount = newCounter.externalCount;
				break;
			}
		}
	}

private:
	atomic<CountedNodePtr> _head; // 스택의 헤드 노드를 가리키는 원자적 포인터
};