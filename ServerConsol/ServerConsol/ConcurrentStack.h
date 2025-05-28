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

template<typename T>

class LockFreeStack {

	struct Node {
		Node(const T& value) :data(value) {

		}

		T data;
		Node* next;
	};


public:
	
	void Push(const T& value) {
		Node* node = new Node(value);


		//node->next = _head;
		////LockFreeStack객체를 동시에 여러 스레드가 사용한다면 해당 과정에서 race condition 발생가능
		//_head = node;

		node->next = _head;

		while (_head.compare_exchange_strong(node->next, node) == false) {
			//compare_exchange_strong 사용으로 race condition 발생 시 재수행
			
		}

	
	}
	// head 읽기
	// head->next 읽기
	// head= head->next
	// data를 추출하여 반환
	// 추출한 노드 삭제

	bool TryPop(T& value) {
		Node* oldHead = _head;

		while (!oldHead && _head.compare_exchange_strong(oldHead, oldHead->next)==false) {

		}
		value = oldHead->data;
		
		
		//delete oldHead;	//삭제 보류(메모리가 계속해서 늘어난다!!!!)
		//다른 스레드에서 먼저 이 동작을 수행해버린다면 접근할 데이터가 없어지게 됨
		//c#, java는 GC가 알아서 메모리 삭제를 진행해주긴 한다.


		return true;
	}

private:
	atomic<Node*> _head;
};