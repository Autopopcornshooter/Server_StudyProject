#include "pch.h"
#include "CoreTLS.h"

thread_local uint32 LThreadId = 0;	//������ ���̵�� 0���� �ʱ�ȭ,

//ThreadManger���� Launch�� ���� �� LThreadId ++1
