#pragma once
#include <mutex>
class User {
	//TODO
};

class UserManager
{
public:
	static UserManager* Instance() {
		static UserManager instance;
		return &instance;

	}

	User* GetUser(int32 ID) {
		lock_guard<mutex> lockGuard(_mutex);

		return nullptr;
	}

	void ProcessSave();

private:
	mutex _mutex;
	
};

