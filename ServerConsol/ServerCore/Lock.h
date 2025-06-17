#pragma once
#include  "Types.h"
/*----------------------

		RW Spin Lock

------------------------*/




// lock�� ���� �����Ͽ� ����ϴ� ����
// 1) ǥ�� mutex�� ��������� lock�� ���� �� ����
// �̹� ���� ���� ���¿��� ȣ��� �ٸ� �Լ����� lock�� ���� �� ����
// 2) �츮�� ���ϴ� ���·� lock�� ����ȭ �� �� �ִ�
// 3) lock�� �����ؼ� deadlock��Ȳ�� ����

// reader-writer lock ��� ����:
// 1) �б�� ���� �� �ϳ��� ���ۿ����� �����ϴ� lock
//		�� �� �ϳ��� ���� Ȯ���� ���� ���� �� ���ʿ��ϰ� �������� �ʰ�


//���� Ȱ��
// int32(32��Ʈ) ���� 16��Ʈ�� ����, ���� 16��Ʈ�� �б� �÷��׸� �ǹ��Ѵ�
//[WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]	
// W : WriteFlag (���� ���� �� ���� ȹ���ϰ� �ִ� ������ ���̵� ����)
// R : ReadFlag (�����ؼ� ����ϰ� �ִ� Read ī��Ʈ�� ����)
class Lock
{
	enum : uint32 {
		ACQUIRE_TIMEOUT_TICK = 100,	//�ִ�� ����� �ð�
		MAX_SPIN_COUNT = 5000,
		WRITE_THREAD_MASK = 0XFFFF'0000,		//���� 16��Ʈ�� �̾ƿ��� ���� ����ũ
		READ_COUNT_MASK = 0X0000'FFFF,			//���� 16��Ʈ�� �̾ƿ��� ���� ����ũ
		// F : 1�� ����
		EMPTY_FLAG = 0X0000'0000				//�ʹ� ����(��� �����Ͱ� 0)
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

