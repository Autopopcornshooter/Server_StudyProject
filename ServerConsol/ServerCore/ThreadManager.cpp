#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"

/*------------------
	ThreadManager
-------------------*/

ThreadManager::ThreadManager()
{
	// Main Thread
	InitTLS();
}

ThreadManager::~ThreadManager()
{
	Join();
}

void ThreadManager::Launch(function<void(void)> callback)		//스레드 추가 콜백함수
{
	LockGuard guard(_lock);

	_threads.push_back(thread([=]()
		{
			InitTLS();
			callback();
			DestroyTLS();
		}));
}

void ThreadManager::Join()										//스레드 마무리 시점까지 대기
{
	for (thread& t : _threads)
	{
		if (t.joinable())
			t.join();
	}
	_threads.clear();
}

void ThreadManager::InitTLS()									//TLS 초기화 함수
{
	static Atomic<uint32> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1);		//스레드 ID부여
}

void ThreadManager::DestroyTLS()								//TLS 삭제 함수
{

}