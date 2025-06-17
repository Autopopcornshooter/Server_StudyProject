#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"

void Lock::WriteLock() {
	//아무도 소유(write) 및 공유(read)하고 있지 않을 때 경합하여 소유권 쟁탈
	//==EMPTY_FLAG 일 때
	const uint32 lockthreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockthreadId) {
		_writeCount++;
		return;
		//동일한 스레드에서 재귀적으로 호출될 시 경합 부분 동작 필요X
		//무조건 성공
	}
	//LThreadId를 16만큼 왼쪽으로 shift
	//write 비트마스크로 왼쪽 비트만을 남김
	const uint32 beginTick = ::GetTickCount();
	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);
	_lockFlag = desired;	//레이스 컨디션 발생가능

	while (true) {
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++) {
			uint32 expected = EMPTY_FLAG;
			if (_lockFlag.compare_exchange_strong(OUT expected, desired)) {
				//경합하여 소유권 얻음
				_writeCount++;
				//WriteLock() 함수가 재귀적으로 호출될 시(동일한 스레드)
				// lock을 잡을 수 있게 해 주고
				//_writeCount를 1 늘려준다.
				return;
			}
		}
		if ((::GetTickCount() - beginTick) >ACQUIRE_TIMEOUT_TICK){
			CRASH("Lock Timeout");
			// 스핀 시간이 최대 대기시간을 초과할 시 임의로 CRASH 발생
		}
		this_thread::yield();
		
	}
}
void Lock::WriteUnlock() {
	const int32 lockCount = --_writeCount;
	if (lockCount == 0) {
		_lockFlag.store(EMPTY_FLAG);
	}
}
void Lock::ReadLock() {

}
void Lock::ReadUnlock() {

}