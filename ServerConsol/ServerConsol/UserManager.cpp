#include "pch.h"
#include "UserManager.h"
#include "AccountManager.h"

void UserManager::ProcessSave() {
	//userlock
	lock_guard<mutex> lockGuard(_mutex);

	Account* account = AccountManager::Instance()->GetAccount(100);
	
}