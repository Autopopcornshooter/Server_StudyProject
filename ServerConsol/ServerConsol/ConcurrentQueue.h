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
		//�ܺο��� �ٷ� pop()�� �Ͽ� �����´ٸ� ����������� crash�߻�Ȯ�����ִ�.
		return true;
	}


	bool Empty() {
		lock_guard<mutex> lock(_mutex);
		return _queue.empty();
	}
	//��Ƽ������ ȯ�濡���� �̷� �˻簡 ũ�� �ǹ̰� ����.(Empty�� Pop���̿� �ٸ� �����尡 ������ ����)

	void WaitPop(T& value) {
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this] {return _queue.empty() == false; });
		//������ ������ �� ���� lock�� Ǯ�� �����·� ��ȯ
		value = std::move(_queue.front());
		_queue.pop();

	}
private:
	mutex _mutex;
	queue<int32> _queue;
	condition_variable _condVar;
};
