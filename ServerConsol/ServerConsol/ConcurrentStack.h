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
//	//���� TryPop�� ���� �ִ��� Ȯ��
//	// �����͸� �и��� �������� �˻��Ͽ� ���� �����忡���� ������̸� _pendinList�� ��� ����
//	// �ٸ� �����忡���� ������̶�� �и��� ��� �ٽ� �������´�
//	//�ٸ� �����忡���� ������̶�� pendingList�� ������ ��带 ����
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
//	//_pendingList�� last��忡 first ��带 �����Ѵ�(last->next=first)
//	void ChainPendingNodeList(Node* first, Node* last) {
//		last->next = _pendingList;
//		while (_pendingList.compare_exchange_strong(last->next, first) == false) {
//
//		}
//	}
//	//ChainPendingNodeList ���� �Լ�
//	void ChainPendingNodeList(Node* node) {a
//		Node* last = node;
//		while (last->next) {
//			last = last->next;
//		}
//		ChainPendingNodeList(node, last);
//	}
//	//ChainPendingNodeList�� ����Ͽ� �ϳ��� ��带 _pendingList�� �����ϴ� �Լ�
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
//			//_head�� node->next�� ���� ������ node->next�� _head�� �ٲٰ� node�� _head�� �ٲ۴�
//			//_head�� node->next�� ������ node�� _head�� �ٲ��� �ʰ� ��� �ݺ��Ѵ�
//		}
//	}
//
//	shared_ptr<T> TryPop() {
//		shared_ptr<Node> oldHead = atomic_load(&_head);
//		
//		while (oldHead && atomic_compare_exchange_weak(&_head, &oldHead, oldHead->next) == false) {
//			//_head�� oldHead�� ���� ������ oldHead�� _head�� �ٲٰ� oldHead�� _head�� �ٲ۴�
//			//_head�� oldHead�� ������ oldHead�� _head�� �ٲ��� �ʰ� ��� �ݺ��Ѵ�
//		}
//
//		if (oldHead == nullptr) {
//			return shared_ptr<T>();
//		}
//		return oldHead->data; // oldHead�� data�� ��ȯ
//
//
//	}
//
//private:
//	shared_ptr<Node> _head;
//	// shared_ptr�� ����Ͽ� ����� �޸� ������ �ڵ����� ó��
//};

template<typename T>
class LockFreeStack {

	struct Node;

	struct CountedNodePtr {
		int32 externalCount = 0;	//��带 ������ �ܺ� ������ ��
		Node* ptr = nullptr;
	};

	struct Node {
		Node(const T& value) : data(make_shared<T>(value)) {
		}
		shared_ptr<T> data;
		atomic<int32> internalCount = 0;	//��� ���ο����� ���� ��
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
		//_head�� �����ϱ� ���� ������ ȹ���ϴ� ��� ���
		while (true) {
			//������ ȹ�� �ܰ�(externalCount�� �� ���� ���� +1�� �����尡 ���ٱ��� ȹ��)
			IncreaseHeadCount(oldHead);
			//�ּ��� externalCount >=2 ���״� ����X(�����ϰ� ������ �� �ִ� ���)
			Node* ptr = oldHead.ptr;
			
			
			//�����Ͱ� ���ٸ�?
			if (ptr == nullptr) {
				return shared_ptr<T>();
			}

			//������ ȹ�� �ܰ� (ptr->next�� head�� �ٲ�ġ�� �� �����尡 �̱�)
			if (_head.compare_exchange_strong(oldHead, ptr->next)) {
				shared_ptr<T> res;
				res.swap(ptr->data);	//���� �ڿ��� ���糪 �̵� ���� ������ ��ȯ

				//external ���۰� 1 => �����Ϸ��� ���� +1(2)
				//internal ���۰� 0 => 

				//�� ���� ���� �� �ִ°�?
				const int32 countIncrease = oldHead.externalCount - 2;

				//fetch_add(x) x����ŭ ���ѵ� ���ϱ� �� ���� ��ȯ
				if (ptr->internalCount.fetch_add(countIncrease) == -countIncrease)
					delete ptr;

				return res;
			}
			else if(ptr->internalCount.fetch_sub(1)==1) {
				//fetch_add(x) x����ŭ �� �� ���� �� ���� ��ȯ
				// ���ǹ��� �����Ѵٸ� �������� ���� ������� �ϳ����̴�
				// �ش� �����尡 �޸� ������ �����Ѵ�.
				
				//�������� ������� �������� ȹ�� ����, �޼����� ���� �Ѵ�(?)
				delete ptr;
			}
		}
	}



private:
	void IncreaseHeadCount(CountedNodePtr& oldCounter) {
		while (true) {
			//�������� ����κ�
			CountedNodePtr newCounter = oldCounter;	
			newCounter.externalCount++;	
			if (_head.compare_exchange_strong(oldCounter, newCounter)) {
				// �������ٽ� CAS�� ���� ������ �����尡 TryPop�� ����
				// ������ _head�� externalCount�� 1����
				//
				oldCounter.externalCount = newCounter.externalCount;
				break;
			}
		}
	}

private:
	atomic<CountedNodePtr> _head; // ������ ��� ��带 ����Ű�� ������ ������
};