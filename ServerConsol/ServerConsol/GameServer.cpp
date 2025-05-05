#include "pch.h"
#include <iostream>
#include "CorePCH.h"

#include <atomic>	
#include <mutex>
#include <thread> //스레드를 생성하기 위한 라이브러리
//다양한 운영체제 환경에 적용 가능하다!

void HelloThread() {
	cout << "Hello Thread" << endl;
}
//
//int main()
//{	
//
//	thread t;	//스레드 객체 먼저 선언 후에 원하는 스레드 할당이 가능하다
//	//현재 get_id()는 0
//	t = thread(HelloThread);	//main 스레드와 독립적으로 실행되는 스레드 t 생성
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
		sum.fetch_add(1);
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
	//위 코드의 답은 0이 아니다
	//sum++ 동작을 수행 할 때 어셈블리어 단계에서는
	//데이터를 레지스터로 꺼내오고 계산 후 다시 입력하는 세 단계를 거친다.
	//때문에 멀티스레드로 위의 동작을 수행 할 시 동작들이 순서대로 동작하지 않으므로
	//원하는 결과를 내지 못하게 된다.
}

*/


//LOCK 실습

vector<int32> v;

mutex m;
//Mutual Exclusive 
//락을 수행한 동작을 진행하는 동안 다른 접근은 불허한다(상호배타적) 
//문제점
//1. lock을 재귀적으로 사용해야 할 경우 있다
//2. lock 호출 후 unlock을 호출하지 않았을 경우


//RAII (Resource Acquisition Is Initialization)
template<typename T>
class LockGuard {
public:

	//생성자에서 뮤텍스를 매개변수로 가져와서 lock() 수행
	LockGuard(T& m) {
		_mutex = &m;
		_mutex->lock();
	}

	//소멸자에서 뮤텍스 unlock() 수행
	~LockGuard() {
		_mutex->unlock();
	}

private:
	T* _mutex;
};


// STL 에서 사용하던 자료구조 컨테이너들은 멀티스레드 환경에서 동작하지 않는다고 보면 편함
void Push() {
	//LockGuard<mutex> lockGuard(m);		//Push()수행시 락 실행
	//락을 거는 범위에 따라 달라지는것들이 많다.
	for (int32 i = 0; i < 10000; i++) {

		LockGuard<mutex> lockGuard(m);		//for문 실행시마다 락 실행

		//unique_lock<mutex> uniqueLock(m, defer_lock);		//qunique_lock 타입


		//uniqueLock.lock();	//객체만 만들어놓고 lock() 수행은 뒤로 미룰 수 있다.


		//m.lock();	//자물쇠 잠그기

		v.push_back(i);

		//m.unlock();	//자물쇠 풀기
	}

	//벡터 캐패시티가 늘어나는 과정 => 
	// 1. 기존 데이터 복사
	// 2. 더 큰 배열에 붙여넣기
	// 3. 기존 배열 삭제
	// 멀티쓰레딩 진행되며 세가지 과정 꼬여 크래시 발생 ex) 더블프리 문제, 같은 위치에 두 원소 입력 문제
	//atomic은 벡터의 세부적인 기능까지는 사용할 수 없기 때문에 automic 타입은 사용할 수 없다
}

int main() {


	thread t1(Push);
	thread t2(Push);

	t1.join();
	t2.join();

	cout << v.size() << endl;		//crash 발생
}