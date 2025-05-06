#pragma once
#include <mutex>

class Account {

	//TODO
};
class AccountManager
{
public:


	static AccountManager* Instance() {
		static AccountManager instance;
		return &instance;

	}

	Account* GetAccount(int32 ID) {

		lock_guard<mutex> lockGuard(_mutex);
		return nullptr;
	}

	void ProcessLogin();

private:
	mutex _mutex;

};

