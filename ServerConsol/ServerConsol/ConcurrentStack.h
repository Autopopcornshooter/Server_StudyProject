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
		Node(const T& value): data(value), next(NULL) {
		}

		T data;
		Node* next;
	};

public:

	void Push(const T& value) {
		Node* node = new Node(value);
		node->next = _head;

		while (node && _head.compare_exchange_strong(node->next, node) == false) {

		}
	}
	bool TryPop(T& value) {
		++_popCount;
		Node* oldHead = _head;

		while (oldHead && _head.compare_exchange_strong(oldHead, oldHead->next) == false) {

		}
		if (oldHead == nullptr) {
			--_popCount;
			return false;
		}
		value = oldHead->data;

		TryDelete(oldHead);

		return true;
	}


	//나만 TryPop을 쓰고 있는지 확인
	// 데이터를 분리한 시점에서 검사하여 현재 스레드에서만 사용중이면 _pendinList의 노드 삭제
	// 다른 스레드에서도 사용중이라면 분리한 노드 다시 돌려놓는다
	//다른 스레드에서도 사용중이라면 pendingList에 삭제할 노드를 저장
	void TryDelete(Node* oldHead) {
		if (_popCount == 1) {
			Node* node = _pendingList.exchange(nullptr);
			if (--_popCount == 0) {
				DeleteNodes(node);
			}
			else if(node) {
				ChainPendingNodeList(node);
			}
			delete oldHead;
		}
		else {
			ChainPendingNode(oldHead);
		}
	}

	void DeleteNodes(Node* node) {
		while (node) {
			Node* next = node->next;
			delete node;
			node = next;
		}

	}
	//_pendingList의 last노드에 first 노드를 연결한다(last->next=first)
	void ChainPendingNodeList(Node* first, Node* last) {
		last->next = _pendingList;
		while (_pendingList.compare_exchange_strong(last->next, first) == false) {

		}
	}
	//ChainPendingNodeList 헬퍼 함수
	void ChainPendingNodeList(Node* node) {a
		Node* last = node;
		while (last->next) {
			last = last->next;
		}
		ChainPendingNodeList(node, last);
	}
	//ChainPendingNodeList를 사용하여 하나의 노드를 _pendingList에 연결하는 함수
	void ChainPendingNode(Node* node) {
		ChainPendingNodeList(node, node);
	}

private:
	atomic<Node*> _head;
	
	atomic<int32> _popCount = 0;

	atomic<Node*> _pendingList;

};