#include <iostream>

using namespace std;

#include "thread_class.h"

int main()
{

///*
    {
    using namespace std;
	using namespace test_namespace;

    unsigned args = 5;
 	const unsigned THREAM_SUM = 5;
	/// test 0 ... 【故意的】complie error ...
// 	thread_base *pThread0 = new thread_base(NULL);
// 	delete pThread0;
	/// test 1 ...
// 	test_thread2* pArray = new test_thread2[THREAM_SUM];
// 	for (size_t i=0; i < THREAM_SUM; i++)
// 		::WaitForSingleObject(pArray[i].handle(), INFINITE);
//
// 	delete[] pArray;

	/// test 2 ...
//	auto_ptr<test_thread> pTest1(new test_thread(&args));
//	pTest1->resume();/// on-or-off
//    if (::WaitForSingleObject(pTest1->handle(), 10000) == WAIT_TIMEOUT)
//        std::cout << " The time-out interval 10s elapsed, and the object's state is nonsignaled.\n";

	/// test 3 ...
//     test_thread2 test2;
//     //test2.resume();/// on-or-off
//     if (::WaitForSingleObject(test2.handle(), 10000) == WAIT_TIMEOUT)
//        std::cout << " The time-out interval 10s elapsed, and the object's state is nonsignaled.\n";

	/// test 4 ...
	/// 【注】test_thread, test_thread2 线程类不适宜结合有构造参数的std::vector使用，
	///  可能因为std::vector构造函数内部的复制构造过程导致句柄[HANDLE]失效，详细原因可进步深究！
    std::vector<test_thread2> thread_vec(5, &args);///or test_thread
    std::cout << " " << thread_vec.size() << "\n";
    for (size_t i=0; i < thread_vec.size(); i++)
    {
        ::WaitForSingleObject(thread_vec[i].handle(), INFINITE);// ?...
        std::cout << "WaitForSingleObject Pass.\n";
    }
	/// test 5 【注】可能打印出现错乱，属于IO(std::cout)中断操作导致。
// 	test_thread threads[THREAM_SUM];
// 	for (size_t i=0; i < THREAM_SUM; i++)
// 		threads[i].resume();
//
// 	for (size_t i=0; i < THREAM_SUM; i++)
// 		::WaitForSingleObject(threads[i].handle(), INFINITE);
//
// 	test_thread2 threads2[THREAM_SUM];
// 	for (size_t i=0; i < THREAM_SUM; i++)
// 		::WaitForSingleObject(threads2[i].handle(), INFINITE);

	//////////////////////////////////////////////////////////////////////////
    std::cout << " all threads are finish !\n";
    }
    /** =   =   =   =   =   =   =   =   =   =*/
    cout << "Hello world!" << endl;
    return 0;
}
