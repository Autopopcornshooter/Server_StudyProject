#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"



class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();		//객체 생성시 스레드매니저 전역변수 생성하여 관리
	}

	~CoreGlobal()
	{
		delete GThreadManager;
	}
} GCoreGlobal;