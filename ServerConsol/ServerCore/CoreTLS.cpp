#include "pch.h"
#include "CoreTLS.h"

thread_local uint32 LThreadId = 0;	//스레드 아이디는 0으로 초기화,

//ThreadManger에서 Launch로 수행 시 LThreadId ++1
