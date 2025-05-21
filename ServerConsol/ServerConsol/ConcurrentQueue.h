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
