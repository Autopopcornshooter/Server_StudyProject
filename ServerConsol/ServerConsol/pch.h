#pragma once

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#ifdef _DEBUG
#pragma comment(lib,"Debug\\ServerCore.lib")//��ó����ܰ迡�� ����

#else
#pragma comment(lib, ""Release\\ServerCore.lib")

#endif // DEBUG



#include "CorePCH.h"