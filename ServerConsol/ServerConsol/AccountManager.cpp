#include "pch.h"
#include "AccountManager.h"
#include "UserManager.h"

void AccountManager::ProcessLogin() {

	lock_guard<mutex> lockGuard(_mutex);
	
	//userlock ��������
	User* user = UserManager::Instance()->GetUser(100);

	//TODO
}