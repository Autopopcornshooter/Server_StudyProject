#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"



class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();		//��ü ������ ������Ŵ��� �������� �����Ͽ� ����
	}

	~CoreGlobal()
	{
		delete GThreadManager;
	}
} GCoreGlobal;