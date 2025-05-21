#include "pch.h"
#include <iostream>
#include "CorePCH.h"
#include "AccountManager.h"
#include "UserManager.h"

#include <thread> //스레드를 생성하기 위한 라이브러리
//다양한 운영체제 환경에 적용 가능하다!

void HelloThread() {
	cout << "Hello Thread" << endl;
}
//
//int main()
//{	
//
//	std::thread t;	//스레드 객체 먼저 선언 후에 원하는 스레드 할당이 가능하다
//	//현재 get_id()는 undefined ID
//	t = std::thread(HelloThread);	//main 스레드와 독립적으로 실행되는 스레드 t 생성
//	//스레드가 살아있는지(할당되어있는지)반환
//
//	int32 count = t.hardware_concurrency();
//	//논리적으로 실행할 수 있는 프로세스 개수? (실질적 기능하는 CPU 코어 개수)
//	cout << count << endl;
//	auto threadID= t.get_id(); //스레드마다 부여되는 ID 반환(구분용)
//
//	cout << "Hello Main" << endl;
//	if(t.joinable())
//	t.join();
//	//t스레드 실행이 끝날 때 까지 main 스레드가 대기하게 해 주는 함수
//	
//	
//}




//automic 타입 실습
#include <atomic>	

/*


atomic<int32> sum = 0;
//멀티스레드 환경에서 공유데이터를 사용할때의 실습

//Atomic = All-Or-Nothing : 동기화 방법 중 하나
//하나의 동작이 일어날 때에 다른 스레드에서 해당 데이터에 대해 접근하는 것을 막는다
//대신 automic 함수 사용 시 실행시간이 늘어난다.
//ex) 
// A에게서 아이템을 제거한다
//B에게 아이템을 추가한다 (이동)
//해당 상황에서 동기화가 진행되지 않으면 둘 중 하나만 실행되는 문제 발생
//이 때 둘 다 실행되던지, 둘다 실행되지 않게 해야 함


int Add() {
	for (int i = 0; i < 1000000; i++) {
		sum.fetch_add(1);	//멀티스레드에서 안전한 방법
		//sum++;
	}
}
int Sub() {
	for (int i = 0; i < 1000000; i++) {
		sum.fetch_add(-1);
		//sum--;
	}

}

int main() {

	thread  t_1(Add);
	thread t_2(Sub);
	t_1.join();
	t_2.join();
	cout << sum << endl;


	//sum++ 동작을 수행 할 때 어셈블리어 단계에서는
	//데이터를 레지스터로 꺼내오고 계산 후 다시 입력하는 세 단계를 거친다.
	//때문에 멀티스레드로 위의 동작을 수행 할 시 동작들이 순서대로 동작하지 않으므로
	//원하는 결과를 내지 못하게 된다.
}

*/

//LOCK 실습

#include <mutex>
//
//vector<int32> v;
//
//mutex m;
////Mutual Exclusive 
////락을 수행한 동작을 진행하는 동안 다른 접근은 불허한다(상호배타적) 
////문제점
////1. lock을 재귀적으로 사용해야 할 경우 있다
////2. lock 호출 후 unlock을 호출하지 않았을 경우(DeadLock)
//
//
////RAII (Resource Acquisition Is Initialization)
//
//template<typename T>
//class LockGuard {
//public:
//
//	//생성자에서 뮤텍스를 매개변수로 가져와서 lock() 수행
//	LockGuard(T& m) {
//		_mutex = &m;
//		_mutex->lock();
//	}
//
//	//소멸자에서 뮤텍스 unlock() 수행
//	~LockGuard() {
//		_mutex->unlock();
//	}
//
//private:
//	T* _mutex;
//};
//
//
//// STL 컨테이너는 자체적으로 동기화를 제공하지 않기 때문에 멀티스레드 환경에서는 사용 시 주의가 필요하다
//void Push() {
//	//LockGuard<mutex> lockGuard(m);		//Push()수행시 락 실행
//	//락을 거는 범위에 따라 달라지는것들이 많다.
//	for (int32 i = 0; i < 10000; i++) {
//
//		LockGuard<mutex> lockGuard(m);		//for문 실행시마다 락 실행
//
//		//unique_lock<mutex> uniqueLock(m, defer_lock);		//qunique_lock 타입
//
//
//		//uniqueLock.lock();	//객체만 만들어놓고 lock() 수행은 뒤로 미룰 수 있다.
//
//
//		//m.lock();	//자물쇠 잠그기
//
//		v.push_back(i);
//
//		//m.unlock();	//자물쇠 풀기
//	}
//
//	//벡터 캐패시티가 늘어나는 과정 => 
//	// 1. 기존 데이터 복사
//	// 2. 더 큰 배열에 붙여넣기
//	// 3. 기존 배열 삭제
//	// 멀티쓰레딩 진행되며 세가지 과정 꼬여 크래시 발생 ex) 더블프리 문제, 같은 위치에 두 원소 입력 문제
//	//atomic은 벡터의 세부적인 기능까지는 사용할 수 없기 때문에 automic 타입은 사용할 수 없다
//}
//
//int main() {
//
//
//	thread t1(Push);
//	thread t2(Push);
//
//	t1.join();
//	t2.join();
//
//	cout << v.size() << endl;		//crash 발생
//}
//




//DeadLock 실습
//
////ProcessSave에서는 Usermanager 락 이후 AccountManger 락 수행
////ProcessLogin()에서는 반대로 수행한다. 서로의 순서가 달라 각자 Lock을 한 상태로
////서로의 스레드를 막고 있는 상태이다.
////해결: lock을 수행하는 순서를 통일한다면 먼저 lock을 수행한 스레드가
//// 나머지 lock동작을 수행 한 후 상대 스레드 실행!
////
////100% 예방할 순 없다..
////mutex에 id를 할당해서 락을 걸 때마다 id 순서를 파악해서 
//// lock 수행하는 예방방법 존재
//// 
////lock 매니저를 구현해서 lock이 발생할 때 순서를 추적해서
////순환이 일어나는 부분을 확인하는 방법도 있다.
//
//void FUNC_1() {
//	for (int32 i = 0; i < 1000; i++) {
//		UserManager::Instance()->ProcessSave();
//	}
//}
//void FUNC_2() {
//	for (int32 i = 0; i < 1000; i++) {
//		AccountManager::Instance()->ProcessLogin();
//	}
//}
//
//int main() {
//	thread t1(FUNC_1);
//	thread t2(FUNC_2);
//
//	t1.join();
//	t2.join();
//
//	cout << "jobs Done" << endl;
//
//	//참고부분
//	mutex m1;
//	mutex m2;
//
//	lock(m1, m2);	
//	//동시에 두개의 뮤텍스를 활용할 때m1.lock(), m2.lock() 순서를 보장해준다
//
//	lock_guard<mutex>(m1, adopt_lock);	
//	//m1은 lock이 걸려있다고 가정(아닐수도 있음) 함수 종료 시 unlock만 수행
//}






//SpinLock 구현	=> lock상태일 때 대기 스레드가 개별자원을 반납하지 않고 무한정 대기
//lock상태가 계속 이어질 경우 CPU 점유율 상승이 오점이 될 수 있음

//
//class SpinLock {
//public:
//
//	void lock() {
//		//_lock을 체크하는 while문과 lock을 시도하는 코드가 두 단계로 나눠져서 발생하는 문제임
//		//두 코드가 동기화(동시에 작동)되어야 한다.
//		//CAS(Comapre-And-Swap)두 코드를 동기하는 것
//
//		bool expected = false;
//		bool desired = true;
//
//		while (_locked.compare_exchange_strong(expected, desired)==false) {
//
//			expected = false;
//
//		}
//		//compare_exchange_strong : CAS 함수 중 하나
//		//풀이
//		/*
//		if (_locked == expected) {
//
//			expected = false;
//			_locked = desired;
//			return true;
//		}
//		else {
//
//			expected = _locked;	//실시간 _locked 상태 체크
//			return false;
//		}
//		*/
//	}
//
//	void unlock() {
//		_locked.store(false);
//	}
//
//private:
//	atomic<bool> _locked = false;
//	//volatile-> 최적화 중지 명령
//	//어셈블리 단계에서 while문 수행 시 최적화 수행의 일종으로 변수체크를 생략하는 경우 있음
//};
//
//
//mutex m;
//int32 sum=0;
//SpinLock spinlock;
//
//void Add() {
//
//	for (int i = 0; i < 100000; i++) {
//		lock_guard<SpinLock> guard(spinlock);
//		sum++;
//	}
//}
//void Sub() {
//
//	for (int i = 0; i < 100000; i++) {
//		lock_guard<SpinLock> guard(spinlock);
//		sum--;
//	}
//}
//
//
//int main() {
//
//	thread t1(Add);
//	thread t2(Sub);
//
//	t1.join();
//	t2.join();
//
//	cout << sum << endl;
//}



//Sleep 실습: 
// 타임슬라이스를 모두 소진하기 전에 자발적으로 커널 레벨로 실행 권한을 넘겨주는 것
//
//class SpinLock {
//public:
//
//	void lock() {
//	
//
//		bool expected = false;
//		bool desired = true;
//
//		while (_locked.compare_exchange_strong(expected, desired) == false) {
//
//			expected = false;
//
//			//this_thread::sleep_for(std::chrono::milliseconds(100));
//			this_thread::sleep_for(0ms);
//			//타임슬라이스 양보 후 100ms 동안 스케줄링되지 않는다
//			//this_thread::yield();
//			//타임슬라이스 양보 후 CPU에 스케줄링 맡김 sleep_for(0ms)와 동일
//		}
//		
//	}
//
//	void unlock() {
//		_locked.store(false);
//	}
//
//private:
//	atomic<bool> _locked = false;
//	
//};
//
//
//mutex m;
//int32 sum = 0;
//SpinLock spinlock;
//
//void Add() {
//
//	for (int i = 0; i < 100000; i++) {
//		lock_guard<SpinLock> guard(spinlock);
//		sum++;
//	}
//}
//void Sub() {
//
//	for (int i = 0; i < 100000; i++) {
//		lock_guard<SpinLock> guard(spinlock);
//		sum--;
//	}
//}
//
//
//int main() {
//
//	thread t1(Add);
//	thread t2(Sub);
//
//	t1.join();
//	t2.join();
//
//	cout << sum << endl;
//}


//Event 실습:
//관리자에게 스레드 실행 순서 보장을 요청하는것
//수행완료된 스레드가 관리자를 통해 이벤트 변경 => 대기중인 스레드가 현재 상태를 읽을 수 있다.
//커널 오브젝트를 사용하여 추가적인 자원을 필요로 하기 때문에
//자주 스레드 전환이 일어나는 부분에서는 지양할 필요가 있다.


//
//#include <Windows.h>
//
//queue<int32>q;
//mutex m;
//HANDLE handle;
// //HANDLE은 커널 레벨 객체를 관리하기 위한 '참조값'
//void Producer() {
//	while (true) {
//
//		{
//			unique_lock<mutex> lock(m);
//			q.push(100);
//		}
//		::SetEvent(handle);
//		
//		this_thread::sleep_for(100ms);
//		//시간이 늘어날수록 Consumer 스레드는 무한정 대기해야 한다.
//		//CPU점유율 사용
//	}
//
//}
//
//void Consumer() {
//	while (true) {
//
//		::WaitForSingleObject(handle, INFINITE);
//		//커널에게 handle이 signal 상태가 될 때 까지 Sleep을 수행하게 된다
//		//signal 상태가 된다면 다음 줄부터 수행
//		//자동리셋 상태일 때: handle을 Non-Signal 상태로 바꿔줌
//		//수동리셋 상태일 때:
//		// ResetEvent(handle); 함수를 호출해 상태를 Non-Signal로 바꿔줘야 함
//
//		//변종으로 커넥션 Variable 방법이 또 있다
//
//		unique_lock<mutex>	lock(m);
//		if (!q.empty()) {
//			int32 data = q.front();
//			q.pop();
//			cout << data << endl;
//		}
//	}
//}
//int main() {
//	//커널 오브젝트
//	//커널 레벨에서 관리하는 오브젝트
//	// 기본적 속성들
//	//Usage Count
//	//Signal / Non-Signal << bool
//	//Auto / Manual <<bool
//
//	handle = ::CreateEvent(NULL/*보안속성관련인자*/, FALSE /*bManualReset*/, FALSE/*bInitialState*/, NULL);
//
//
//	thread t1(Producer);
//	thread t2(Consumer);
//
//	t1.join();
//	t2.join();
//
//	::CloseHandle(handle);		//메모리 누수 방지
//
//}


//Condition Variable 실습


//#include <Windows.h>
//
//queue<int32>q;
//mutex m;
//HANDLE handle;
//
//
//condition_variable cv;		//mutex 와 상호작용
////User-Level Object (커널오브젝트가 아니다)
////동일한 프로그램 내부에서만 사용 가능
//
//void Producer() {
//	while (true) {
//		
//		//
//
//
//		{
//			unique_lock<mutex> lock(m);
//			q.push(100);
//		}
//
//		cv.notify_one(); //wait중인 스레드가 있다면 1개를 깨운다(단 하나?)
//		
//		//this_thread::sleep_for(100ms);
//
//	}
//
//}
//
//void Consumer() {
//	while (true) {
//		unique_lock<mutex>	lock(m);	//lock 객체만 생성
//		cv.wait(lock, []() {return q.empty() == false; });
//		// 1. lock 시도
//		// 2. wait 함수 조건 확인
//		// 3-1. 만족 시 코드 이어서 진행
//		// 3-2. 불만족 시 Lock을 풀고 대기상태로 전환
//		// **wait함수에서는 임의로 lock을 풀어줘야 할 수 있어야 하기 때문에 인자로 unique_lock을 사용한다
//		// notify_one()이 호출되었을 때 깨어난 상황이 원하는 조건이 아닌 상황일 수 있다(Spurious Wakeup)
//		// => notify_one() 시점에서 lock()을 수행하는것이 아니기 때문
//
//		//while (!q.empty()) 
//		 {
//			int32 data = q.front();
//			q.pop();
//			cout << q.size() << endl;
//		}
//	}
//}
//int main() {
//
//
//	//handle = ::CreateEvent(NULL/*보안속성관련인자*/, FALSE /*bManualReset*/, FALSE/*bInitialState*/, NULL);
//
//
//	thread t1(Producer);
//	thread t2(Consumer);
//
//	t1.join();
//	t2.join();
//
//	//::CloseHandle(handle);
//
//}


//Future 실습
//std::future 미래 객체
//단발성 함수를 비동기 방식으로 실행하고 싶을 때
//특히 한 번 발생하는 이벤트에 유용하다.


//
//#include <future>
//
//int64 result;
//
//int64 Calculate() {
//	int64 sum = 0;
//
//	for (int32 i = 0; i < 1000000; i++) {
//		sum += i;
//	}
//
//	return sum;
//}
//
//void PromiseWorker(std::promise<string>&& promise) {
//	promise.set_value("Secret Message");
//}
//
//void TaskWorker(std::packaged_task<int64(void)>&& task) {
//	
//	task();
//	//결과물을 리턴방식으로 받지 않는다
//	//객체 자체를 통해서 결과물을 참조할 수 있다
//}
//
//int main() {
//	// 동기 실행(synchronous)
//	//함수 호출 시 모든 코드를 순서대로 수행
//	//문제발생 : 코드의 크기가 엄청나게 크다면 코드를 모두 수행할동안 다른 동작 불가
//	int64 sum_1 = Calculate();
//
//	thread t(Calculate);	
//	//반환받으려면 전역변수 해야된다.(번거로움)
//	// 간단한 코드를 수행하는데 스레드까지 직접 생성해야 해야 한다
//
//	{
//		// 1) deferred -> lazy evaluation 지연 실행(나중에 future.get()호출 시 실행)
//		// 2) async -> 별도의 스레드를 만들어서 실행(비동기 방식으로 실행하다가 future.get()시점에 값 반환
//		// 3) deferred | async	-> 둘 중 알아서 골라 실행(?)
//		std::future<int64> future = std::async(std::launch::async, Calculate);
//
//		//TODO
//
//		std::future_status status = future.wait_for(1ms);
//		if (status == future_status::ready) {
//			//수행이 끝났는지 판단
//		}
//
//		int64 sum = future.get();
//
//
//		//std::promise를 통한 future 사용방법
//		{
//			//미래에 결과물 반환을 약속하는 것
//			std::promise<string>promise;
//			std::future<string> future=promise.get_future();
//
//			thread t(PromiseWorker, std::move(promise));	
//			//스레드 PromiseWorker t 로 promise 스레드 소유권 이동
//
//			string message = future.get();
//			cout << message << endl;
//
//			t.join();
//		}
//
//		//std::packaged_task
//
//		{
//			std::packaged_task<int64(void)>task(Calculate);
//			
//			std::future<int64> future = task.get_future();
//
//			std::thread t(TaskWorker, std::move(task));
//			//task는 함수를 수동으로 호출하여 실행하거나 thread로 넘겨야 한다.
//			int64 sum = future.get();
//			cout << sum << endl;
//
//			t.join();
//		}
//		
//		
//		//mutex, CV를 써 lock 걸지 않고 단순한 함수 처리에 유용
//		// 1) async
//		//	원하는 함수를 비동기적으로 실행
//		// 2) promise
//		//	결과물을 promise 를 통해 future로 받아줌
//		// 3) packaged_task
//		//	원하는 함수의 실행 결과를 packaged_task를 통해 future로 받아줌
//
//
//
//		//{
//		//	class Knight
//		//	{
//		//	public:
//		//		int64 GetHP() { return 100; }
//
//		//	};
//
//		//	Knight knight;
//		//	std::future<int64> future2 = std::async(std::launch::async, Knight::GetHP, knight);
//		//	//멤버 변수 호출 시 의존하는 클래스 객체를 인자로 넣어줘야함
//		//}
//
//	}
//
//	t.join();
//
//
//}


//Cache(캐시) 실습
//캐시 지역성: 속도는 캐시 지역성이 높을수록 빠르다
// 1) 시간 지역성(Temporal Locality) : 최근에 사용했던 데이터의 재참조 가능성이 높음
// 2) 공간 지역성(Spatial Locality) : 최근에 사용했던 데이터와 인접한 데이터가 참조될 가능성이 높음

//
//#include <Windows.h>
//
//int32 buffer[10000][10000];
//
//int main() {
//	memset(buffer, 1, sizeof(buffer));
//	
//
//
//	{
//		uint64 start = GetTickCount64();
//
//		int64 sum = 0;
//
//		for (int32 i = 0; i < 10000; i++) {
//			for (int32 j = 0; j < 10000; j++) {
//				sum += buffer[i][j];
//			}
//		}
//
//		uint64 end = GetTickCount64();
//		cout << "Elapsed Tick " << (end - start) << endl;
//		cout << sum << endl;
//	}
//	//첫번째 인덱스 그룹의 원소들 참조(공간 지역성 높음)
//
//	{
//		uint64 start = GetTickCount64();
//
//		int64 sum = 0;
//		for (int32 i = 0; i < 10000; i++) {
//			for (int32 j = 0; j < 10000; j++) {
//				sum += buffer[j][i];
//			}
//		}
//
//		uint64 end = GetTickCount64();
//		cout << "Elapsed Tick " << (end - start) << endl;
//		cout << sum << endl;
//	}
//	//각 첫번째 인덱스 그룹의 첫번째 원소들부터 참조(공간 지역성 낮음)
//}



//CPU 파이프라인에 대한 실습

//가시성의 문제:
// 1) 스레드는 각기 다른 코어가 연산을 수행함
// 2) 각 코어는 데이터에 대한 빠른 접근을 위해 캐시 사용
// 3) 변경된 데이터가 캐시에는 store 되었지만 메모리에는 업데이트되지 않음
// 4) 다른 스레드는 메모리에서 변경 이전의 데이터를 load하게 됨
// 해결책: 변수에 volatile 처리를 해주면 된다.(캐시에 저장안하고 메모리에 바로 store)

//코드 재배치의 문제
// CPU와 컴파일러에서 시간 단축의 일환으로 코드 순서가 바뀔 수 있다
// CPU 파이프라인 단계 Fetch, Decode, Execute, Write-back 를 진행한다
// (단일 스레드 기준)코드 순서를 바꾸면 효율적으로 동작할것이라고 판단한다면 코드 순서가 바뀔 수 있다.
// 이 문제는 프로그래머가 정확히 파악할 수 없다.

//
//int32 x = 0;
//int32 y = 0;
//int32 r1 = 0;
//int32 r2 = 0;
//
//volatile bool ready;
//
//void Thread_1() {
//
//	while (!ready)
//		;
//	
//
//	y = 1;	//Store y
//	r1 = x;	//Load x
//
//}
//
//void Thread_2() {
//
//	while (!ready)
//		;
//
//	x = 1;	//Store x
//	r2 = y;	//Load y
//
//}
//
//int main() {
//
//	int32 count = 0;
//
//	while (true) {
//		ready = false;
//		count++;
//
//		x = y = r1 = r2 = 0;
//		
//		thread t1(Thread_1);
//		thread t2(Thread_2);
//		
//		ready = true;
//
//		t1.join();
//		t2.join();
//
//		if (r1 == 0 && r2 == 0) {
//			break;		//정상적인 파이프라인이라면 만족되지 않을 조건문
//		}
//	}
//
//	cout << count << " 번만에 빠져나옴" << endl;
//}




//메모리 모델(Memory Model) 실습

// 원자적 연산에 의해 객체에 대한 1회 이상의 값 수정이 있을 시
// 모든 쓰레드는 동일 객체에 대해서 동일한 수정 순서를 관찰한다
//
//atomic<bool> flag;
//int main() {
//	flag = false;
//
//	cout << flag.is_lock_free() << endl;	
//	//락을 걸 필요가 없는지 판단 true: lock을 걸지 않음, false : lock이 필요함
//
//	flag.store(true,memory_order_seq_cst);
//	
//	bool val = flag.load();
//
//	//이전 flag값을 prev에 넣고, flag값 수정
//	{
//		
//		/*bool prev = flag;		
//		flag = true;*/
//		//prev의 유효성을 확보하기 위해 위의 두 과정이 원자적으로 일어나야 함
//		bool prev = flag.exchange(true);
//		//exchange: 인자로 값 변경, 이전값을 반환
//	}
//
//	//CAS (Compare-And-Swap) 조건부 수정(원자적 수행)
//	{
//		bool expected = false;
//		bool desired = true;
//
//		flag.compare_exchange_strong(expected, desired);
//		// Spurious Failure이 발생해도(false반환) 성공하는 상황이 나올 때 까지 코드 반복
//		// compare_exchage_weak 대비 부하가 크다
//
//		flag.compare_exchange_weak(expected, desired);
//		// Spurious Failure이 발생하면 false 반환
//	}
//	
//	
//}


//메모리 모델 3가지 실습 
//
//void Producer() {
//	value = 10;
//
//	ready.store(true, memory_order_seq_cst);
//}
//
//void Consumer() {
//
//	while (ready.load(memory_order_seq_cst) == false);
//
//	
//}
//
//atomic<bool> ready;
//int32 value;
//int main() {
//	ready = false;
//	value = 0;
//
//	thread t1(Producer);
//	thread t2(Consumer);
//
//
//	t1.join();
//	t2.join();
//
//	//메모리 모델 (정책)
//	// 1) Sequeantially Consistent (memory_order_seq_cst)
//	//		컴파일러 최적화 여지가 적음 = 코드 재배치 확률이 적다 = 직관적
//	//		가시성 문제와 코드 재배치 문제를 한번에 해결이 가능하다.
//
//	// 2) Acquire-Release (acquire, release)
//	//		relase 이전의 메모리 명령들이, 해당 명령을 기준으로 앞과 뒤의 코드가 서로 재배치되는것을 금지
//	//		acquire 이후에 접근하는 데이터들의 가시성을 확보해준다
//	//		
//
//	// 3) Relaxed (relaxed)
//	//		컴파일러 최적화 여지가 많음 = 코드 재배치 확률 높음 = 덜 직관적
//	//		매우 자유롭다 = 가시성문제 X, 코드 재배치 문제X
//	//		동일 객체 동일 수정 순서만 지켜진다.(가장 기본적인 조건)
//
//	// 인텔과 AMD CPU 칩은 순차적 일관성을 보장한다
//	// seq_cst 를 사용해도 부하의 차이 없음
//	// ARM의 경우 부하의 차이가 꽤 있다고 함
//}
//
////찾아볼것: atomic에 대한것
//

//
////Thread Local Storage 실습
//
//thread_local int32 LThreadID = 0;
// //TLS 변수는 스레드 생성시 복사되어 각 스레드 내부에서만 사용할 수 있다.
////로그 버퍼와 같이 스레드마다 독립적인 데이터를 유지해야 하는 경우
////성능상의 이유로 락 없이 상태를 유지해야 할 때
//
//void ThreadMain(int32 threadID) {
//	LThreadID = threadID;
//
//	while (true) {
//		cout << "Hi! I am THread " << LThreadID << endl;
//		this_thread::sleep_for(1s);
//	}
//}
//
//int main() {
//
//	vector<thread> threads;
//
//	for (int32 i = 0; i < 10; i++) {
//		int32 threadID = i + 1;
//		threads.push_back(thread(ThreadMain, threadID));
//	}
//
//	for (thread& t : threads)
//		t.join();
//	
//}

//Lock-Based Stack/Queue 실습


#include "ConcurrentQueue.h"
#include "ConcurrentStack.h"
LockQueue<int32> q;
LockStack<int32> s;

void Push() {
	while (true) {

		int32 value = rand() % 100;

		q.Push(value);

		this_thread::sleep_for(10ms);
	}
}

void Pop() {
	while (true) {

		int32 data = 0;
		if (q.TryPop(data))
			cout << data << endl;

	}
}

int main() {

	thread t1(Push);
	thread t2(Pop);

	t1.join();
	t2.join();
}


