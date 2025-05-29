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
		//�ܺο��� �ٷ� pop()�� �Ͽ� �����´ٸ� ����������� crash�߻�Ȯ�����ִ�.
		return true;
	}


	bool Empty() {
		lock_guard<mutex> lock(_mutex);
		return _stack.empty();
	}
	//��Ƽ������ ȯ�濡���� �̷� �˻簡 ũ�� �ǹ̰� ����.(Empty�� Pop���̿� �ٸ� �����尡 ������ ����)

	void WaitPop(T& value) {
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this] {return _stack.empty() == false; });
		//������ ������ �� ���� lock�� Ǯ�� �����·� ��ȯ
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


	//���� TryPop�� ���� �ִ��� Ȯ��
	// �����͸� �и��� �������� �˻��Ͽ� ���� �����忡���� ������̸� _pendinList�� ��� ����
	// �ٸ� �����忡���� ������̶�� �и��� ��� �ٽ� �������´�
	//�ٸ� �����忡���� ������̶�� pendingList�� ������ ��带 ����
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
	//_pendingList�� last��忡 first ��带 �����Ѵ�(last->next=first)
	void ChainPendingNodeList(Node* first, Node* last) {
		last->next = _pendingList;
		while (_pendingList.compare_exchange_strong(last->next, first) == false) {

		}
	}
	//ChainPendingNodeList ���� �Լ�
	void ChainPendingNodeList(Node* node) {a
		Node* last = node;
		while (last->next) {
			last = last->next;
		}
		ChainPendingNodeList(node, last);
	}
	//ChainPendingNodeList�� ����Ͽ� �ϳ��� ��带 _pendingList�� �����ϴ� �Լ�
	void ChainPendingNode(Node* node) {
		ChainPendingNodeList(node, node);
	}

private:
	atomic<Node*> _head;
	
	atomic<int32> _popCount = 0;

	atomic<Node*> _pendingList;

};