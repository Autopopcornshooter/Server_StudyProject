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
