#pragma once
#include  "Types.h"
/*----------------------

		RW Spin Lock

------------------------*/




// lock을 직접 구현하여 사용하는 이유
// 1) 표준 mutex는 재귀적으로 lock을 잡을 수 없다
// 이미 락이 잡힌 상태에서 호출된 다른 함수에서 lock을 잡을 수 없다
// 2) 우리가 원하는 형태로 lock을 최적화 할 수 있다
// 3) lock을 추적해서 deadlock상황을 방지

// reader-writer lock 사용 이유:
// 1) 읽기와 쓰기 중 하나의 동작에서만 동작하는 lock
//		둘 중 하나의 동작 확률이 아주 낮을 때 불필요하게 경합하지 않게


//변수 활용
// int32(32비트) 상위 16비트는 쓰기, 하위 16비트는 읽기 플래그를 의미한다
//[WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]	
// W : WriteFlag (쓰기 동작 중 락을 획득하고 있는 스레드 아이디 기입)
// R : ReadFlag (공유해서 사용하고 있는 Read 카운트를 기입)
class Lock
{
	enum : uint32 {
		ACQUIRE_TIMEOUT_TICK = 100,	//최대로 대기할 시간
		MAX_SPIN_COUNT = 5000,
		WRITE_THREAD_MASK = 0XFFFF'0000,		//상위 16비트를 뽑아오기 위한 마스크
		READ_COUNT_MASK = 0X0000'FFFF,			//하위 16비트를 뽑아오기 위한 마스크
		// F : 1로 세팅
		EMPTY_FLAG = 0X0000'0000				//초반 상태(모든 데이터가 0)
	};

public:
	void WriteLock();
	void WriteUnlock();
	void ReadLock();
	void ReadUnlock();

private:
	Atomic<uint32> _lockFlag = EMPTY_FLAG;
	uint16 _writeCount = 0;
};

