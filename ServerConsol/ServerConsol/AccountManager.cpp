#include "pch.h"
#include "AccountManager.h"
#include "UserManager.h"

void AccountManager::ProcessLogin() {

	lock_guard<mutex> lockGuard(_mutex);
	
	//userlock 간접수행
	User* user = UserManager::Instance()->GetUser(100);

	//TODO
}