
#include <iostream>
#include <cstdio>
#include "win32_sync_func.h"

namespace junkun
{
using namespace std;
#define THREAD_COUNT    10
#define WRITE_TIMES     5

locker* s_lock_ptr = NULL;


DWORD WINAPI WriteToDatabase( LPVOID );

void test_win32_sync_func(int num)
{
    HANDLE aThread[THREAD_COUNT];
    DWORD ThreadID;

    // Create a locker with no initial owner

    s_lock_ptr = create_locker(_T("test_win32_sync_func"), num);// _T("")

    if (s_lock_ptr == NULL || !s_lock_ptr->is_valid() )
    {
        //printf("create_locker error: %d, s_lock_ptr:%p, handle:%d.\n", s_lock_ptr->error(), s_lock_ptr, s_lock_ptr->handle());
        cout_trace("create_locker error:" << s_lock_ptr->error());
        return;
    }

    // Create worker threads

    for(unsigned i=0; i < THREAD_COUNT; i++ )
    {
        aThread[i] = CreateThread(
                         NULL,       // default security attributes
                         0,          // default stack size
                         (LPTHREAD_START_ROUTINE) WriteToDatabase,
                         NULL,       // no thread function arguments
                         0,          // default creation flags
                         &ThreadID); // receive thread identifier

        if( aThread[i] == NULL )
        {
            //printf("CreateThread error: %d\n", GetLastError());
            cout_trace("CreateThread error:" << ::GetLastError());
            return;
        }
    }

    // Wait for all threads to terminate

    WaitForMultipleObjects(THREAD_COUNT, aThread, TRUE, INFINITE);

    // Close thread and locker handles

    for(unsigned i=0; i < THREAD_COUNT; i++ )
        CloseHandle(aThread[i]);

    destory_locker(s_lock_ptr);
}

DWORD WINAPI WriteToDatabase( LPVOID lpParam )
{

    // Request ownership of locker.

    for(unsigned i = 0; i < WRITE_TIMES; ++i)
    {
//        bool lock_ret =  s_lock_ptr->lock();
//        assert (lock_ret);
        scoped_lock_t<locker> lock(*s_lock_ptr); /// or: scoped_lock lock(*s_lock_ptr)

        cout_trace("lock.");
        //cout_trace("writing to database..."); // for critical_section (locker)
/**
        switch (s_lock_ptr->get_wait_ret())
        {
            // The thread got ownership of the locker
        case WAIT_OBJECT_0:
            //__try
        {
            // TODO: Write to the database
            //printf("Thread %d writing to database...\n", GetCurrentThreadId());
            cout_trace("writing to database...");
        }

        //__finally
        {
            // Release ownership of the locker object
//            lock_ret =  s_lock_ptr->unlock();
//            assert (lock_ret);
            //if (! lock_ret) {} //// Deal with error.
        }
        break;

        // The thread got ownership of an abandoned locker
        case WAIT_ABANDONED:
        default:
            return FALSE;
        } /***/
    }
    return TRUE;
}

}//namespace junkun
