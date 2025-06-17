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
	const uint32 beginTick = ::GetTickCount();
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
		if ((::GetTickCount() - beginTick) >ACQUIRE_TIMEOUT_TICK){
			CRASH("Lock Timeout");
			// ���� �ð��� �ִ� ���ð��� �ʰ��� �� ���Ƿ� CRASH �߻�
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