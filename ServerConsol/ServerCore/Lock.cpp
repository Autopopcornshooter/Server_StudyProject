#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"

void Lock::WriteLock() {
	//�ƹ��� ����(write) �� ����(read)�ϰ� ���� ���� �� �����Ͽ� ������ ��Ż
	//==EMPTY_FLAG �� ��
	const uint32 lockthreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockthreadId) {
		_writeCount++;
		return;
		//������ �����忡�� ��������� ȣ��� �� ���� �κ� ���� �ʿ�X
		//������ ����
	}
	//LThreadId�� 16��ŭ �������� shift
	//write ��Ʈ����ũ�� ���� ��Ʈ���� ����
	const uint32 beginTick = GetTickCount64();
	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);
	_lockFlag = desired;	//���̽� ����� �߻�����

	while (true) {
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++) {
			uint32 expected = EMPTY_FLAG;
			if (_lockFlag.compare_exchange_strong(OUT expected, desired)) {
				//�����Ͽ� ������ ����
				_writeCount++;
				//WriteLock() �Լ��� ��������� ȣ��� ��(������ ������)
				// lock�� ���� �� �ְ� �� �ְ�
				//_writeCount�� 1 �÷��ش�.
				return;
			}
		}
		if ((GetTickCount64() - beginTick) >ACQUIRE_TIMEOUT_TICK){
			CRASH("Lock Timeout");
			// ���� �ð��� �ִ� ���ð��� �ʰ��� �� ���Ƿ� CRASH �߻�
		}
		this_thread::yield();
		
	}
}
void Lock::WriteUnlock() {
	//readlock�� �ٸ� �����嵵 ���ÿ� lock���� ���� �� �ֱ⿡
	//readlock�� count�� 0�� �ƴ� �� wirtelock�� ���� �� ����.
	if ((_lockFlag.load() & READ_COUNT_MASK) != 0) {
		CRASH("INVALID_UNLOCK_ORDER");
	}

	const int32 lockCount = --_writeCount;
	if (lockCount == 0) {
		_lockFlag.store(EMPTY_FLAG);
	}
}
void Lock::ReadLock() {
	//������ �����尡 �����ϰ� �ִٸ� ������ ����
	const uint32 lockthreadId = (_lockFlag.load() & READ_COUNT_MASK);
	if (LThreadId == lockthreadId) {
		_lockFlag.fetch_add(1);
		return;
		//������ �����忡�� ��������� ȣ��� �� ���� �κ� ���� �ʿ�X
		//������ ����
	}
	//�ƹ��� �����ϰ� ���� ���� �� �����Ͽ� ���� ī��Ʈ�� �ø���
	const uint32 beginTick = GetTickCount64();
	while (true) {

		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++) {
			uint32 expected = (_lockFlag.load() & READ_COUNT_MASK);
			//CAS ���� �� ����_lockFlag�� expected���� ���ٸ�
			//write�κ��� ����ִٴ� ������ ���� ����
			if (_lockFlag.compare_exchange_strong(expected, expected + 1)) {
				//load ���� 1�߰�
				return;
			}
		}
		if (GetTickCount64() > ACQUIRE_TIMEOUT_TICK) {
			CRASH("Lock Timeout");
		}
		this_thread::yield();
	}
}
void Lock::ReadUnlock() {
	if ((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0) {
		CRASH("MULTIPLE_UNLOCK");
	}
}