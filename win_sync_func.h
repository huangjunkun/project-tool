#ifndef WIN32_SYNC_FUNC_H_INCLUDED
#define WIN32_SYNC_FUNC_H_INCLUDED

#include <iostream>
#include <limits.h>
#include <windows.h>
#include "c_single_file.h"
//#include <stl/_alloc.h>


/**
** Դ�ļ��� win32_sync_func.h, win32_sync_func_test.cpp
** ����˵����
** #1���ȳ��������locker��Ȼ��������ͬ������mutex��event��critical_section��semaphore�Ŀ�õ�������ʵ������Ҫͬ�����������
** ע�⣺��������critical_section��WIN�ں˶������������������ں˶����������Ա������΢��ͬ��
** #2��������һ����ʵ��ģʽ��ģ���Ƶ�������ɣ���win_sync_functor��ͬ���̳л���locker������ݲ�ͬ�����������岻ͬ������ͣ�
** _Constructor, _Destoryer, _Locker, _Unlocker������������win_sync_functor��ģ�����������������͵��Ƶ���
** ��ʵ���Ƶ�����mutex2���������������event2��critical_section2��semaphore2����
** #3��ģ������scoped_lock_t���ڸ������ϲ�ͬ�����͵�ʹ�á����Ͼ���ʵ��ϸ��������롣
** #4������ϵͳ��windows 7 רҵ��
** ����+���Ի�����VS2003 + CodeBlocks10.02

** ���ߣ�junkun huang  e-mail��huangjunkun@gmail.com
** ���ڣ�2011-10-16 /
*/

namespace junkun
{
using namespace std;
using namespace junkun;

/// --- /// --- /// --- /// ---

#ifndef LIMIT_COPY_AND_ASSIGN_CTOR
#define LIMIT_COPY_AND_ASSIGN_CTOR(classname) \
    private: \
    classname(const classname& other) {} \
    classname& operator=(const classname& other) { return *this; }
#endif

class locker
{
    LIMIT_COPY_AND_ASSIGN_CTOR(locker)

public:
    locker()
        : _handle(0), _wait_time(INFINITE)
    {}

    locker(const string_t& name)
        : _handle(0), _name(name), _wait_time(INFINITE)
    {}

    virtual ~locker() {}

    virtual bool lock() = 0;
    virtual bool unlock() = 0;

    //operator HANDLE() { return _handle; }
    virtual bool is_valid() const
    {
        return (0 != _handle);
    }
    HANDLE handle() const
    {
        return _handle;
    }
    string_t name() const
    {
        return _name;
    }
    int get_wait_ret() const
    {
        return _wait_ret;
    }
    unsigned get_wait_time() const
    {
        return _wait_time;
    }
    void set_wait_time(unsigned wait_time)
    {
        _wait_time = wait_time;
    }
    int error() const
    {
        return ::GetLastError();
    }

protected:
    HANDLE      _handle;
    string_t    _name;
    unsigned    _wait_time;
    int         _wait_ret;
};

class mutex : public locker
{
public:
    mutex()
        : locker()
    {
        _handle = ::CreateMutex(NULL,              // default security attributes
                                FALSE,             // initially not owned
                                NULL);             // unnamed mutex
    }

    mutex(const string_t& name)
        : locker(name)
    {
        _handle = (::CreateMutex(NULL, FALSE, name.c_str()));
    }

    mutex(LPSECURITY_ATTRIBUTES lpMutexAttributes,
          BOOL bInitialOwner,
          LPCTSTR lpName)
        : locker(lpName)
    {
        _handle = (::CreateMutex(lpMutexAttributes, bInitialOwner, lpName));
    }

    ~mutex()
    {
        if (_handle) CloseHandle(_handle);
    }
    bool lock()
    {
        if (_handle)
        {
            _wait_ret = ::WaitForSingleObject(
                            _handle,        // handle to mutex
                            _wait_time);  // no time-out interval

            if (WAIT_OBJECT_0 == _wait_ret)
                return true;
        }
        return false;
    }
    bool unlock()
    {
        return ::ReleaseMutex(_handle);
    }
};

class event : public locker
{
public:
    event()
        :locker()
    {
        _handle = (::CreateEvent(NULL,     // default security attributes
                                 FALSE,    // auto-reset event
                                 TRUE,     // initial state is signaled
                                 NULL));   // object not named
    }

    event(const string_t& name)
        : locker(name)
    {
        _handle = (::CreateEvent(NULL, FALSE, TRUE, name.c_str()));
    }

    event(LPSECURITY_ATTRIBUTES lpEventAttributes,
          BOOL bManualReset,
          BOOL bInitialState,
          LPCTSTR lpName)
        : locker(lpName)
    {
        _handle = (::CreateEvent(lpEventAttributes, bManualReset, bInitialState, lpName));
    }

    ~event()
    {
        if (_handle)
            CloseHandle(_handle);
    }
    bool lock()
    {
        if (_handle)
        {
            _wait_ret = ::WaitForSingleObject(_handle,    // handle to event
                                              _wait_time);// no time-out interval
            ResetEvent(_handle);
            if (WAIT_OBJECT_0 == _wait_ret)
                return true;
        }
        return false;
    }
    bool unlock()
    {
        return ::SetEvent(_handle);
    }
    bool reset()
    {
        return ::ResetEvent(_handle);
    }
};
class critical_section : public locker
{
    // Note:A critical section object must be deleted before it can be reinitialized.
    // Initializing a critical section that has already been initialized results in undefined behavior.
    void _do_ctor()
    {
        ::InitializeCriticalSection(&_critical_section);
    }
public:
    critical_section()
        : locker()
    {
        _do_ctor();
    }
    critical_section(const string_t& name)
        : locker(name)
    {
        _do_ctor();
    }
    virtual ~critical_section()
    {
        ::DeleteCriticalSection(&_critical_section);
    }

    bool lock()
    {
        ::EnterCriticalSection( &_critical_section);
        return true;
    }
    bool unlock()
    {
        ::LeaveCriticalSection(&_critical_section);
        return true;
    }
    bool try_lock()
    {
        return (TRUE == TryEnterCriticalSection(&_critical_section));
    }
    virtual bool is_valid() const
    {
        return true;
    }

private:
    CRITICAL_SECTION _critical_section;
};

class semaphore : public locker
{

public:
    semaphore()
        : locker()
    {
        _handle = ::CreateSemaphore(NULL,           // default security attributes
                                    1,              // initial count
                                    LONG_MAX,       // maximum count
                                    NULL);          // unnamed semaphore
    }

    semaphore(const string_t& name)
        : locker(name)
    {
        _handle = (::CreateSemaphore(NULL, 1, LONG_MAX, name.c_str()));
    }

    semaphore(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
              LONG lInitialCount,
              LONG lMaximumCount,
              LPCTSTR lpName)
        : locker(lpName)
    {
        _handle = (::CreateSemaphore(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName));
    }

    ~semaphore()
    {
        if (_handle) CloseHandle(_handle);
    }
    bool lock()
    {
        if (_handle)
        {
            _wait_ret = ::WaitForSingleObject(_handle,      // handle to semaphore
                                              _wait_time);  // no time-out interval

            if (WAIT_OBJECT_0 == _wait_ret)
                return true;
        }
        return false;
    }
    bool unlock()
    {
        assert (_handle);
        return ::ReleaseSemaphore(_handle,      // handle to semaphore
                                  1,            // increase count by one
                                  NULL);        // not interested in previous count;
    }
    bool release(long r_count, long* pre_count_ptr = NULL)
    {
        return ::ReleaseSemaphore(_handle,      // handle to semaphore
                                  r_count,            // increase count by one
                                  pre_count_ptr);        // not interested in previous count;
    }

private:
    //unsigned    _max_count;
};


template <class _Locker>
class scoped_lock_t
{
public:
    scoped_lock_t( _Locker& mutex_obj )
        : _locker(mutex_obj)
    {
        bool lock_ret =  _locker.lock();
        assert (lock_ret);
    }
    ~scoped_lock_t()
    {
        bool lock_ret = _locker.unlock();
        assert (lock_ret);
    }
private:
    _Locker& _locker;
};

typedef scoped_lock_t<locker> scoped_lock;


template <  class _Constructor,
class _Destoryer,
class _Locker,
class _Unlocker>
class win_sync_functor : public locker
{
public:
    win_sync_functor()
        : locker()
    {
        _handle = _Constructor()(_T(""));
    }

    win_sync_functor(const string_t& name)
        : locker(name)
    {
        _handle = _Constructor()(name);
    }

    virtual ~win_sync_functor()
    {
        if (_handle) _Destoryer()(_handle);
    }
    bool lock()
    {
        //cout_trace("begin lock.");
        if (_handle)
        {
            _wait_ret = _Locker()(_handle, _wait_time);
            return WAIT_OBJECT_0 == _wait_ret;
        }
        return false;
    }
    bool unlock()
    {
        return _Unlocker()(_handle);
    }
};

namespace mutex_space
{

struct mutex_ctor
{
    HANDLE operator() (const string_t& name)
    {
        LPCTSTR lpName = name.c_str();
        if (name.empty())
            lpName = NULL;
        return (::CreateMutex(NULL, FALSE, lpName));
    }
};
struct mutex_dtor
{
    bool operator() (HANDLE h)
    {
        return ::CloseHandle(h);
    }
};
struct mutex_locker
{
    int operator() (HANDLE hHandle, DWORD dwMilliseconds)//
    {
        return ::WaitForSingleObject(hHandle, dwMilliseconds);
    }
};
struct mutex_unlocker
{
    bool operator() (HANDLE h)
    {
        return ::ReleaseMutex(h);
    }
};
}/// namespace mutex_space

typedef win_sync_functor<mutex_space::mutex_ctor, mutex_space::mutex_dtor, mutex_space::mutex_locker, mutex_space::mutex_unlocker> mutex2;

/// implement event2 critical_section2, semaphore2 -->>  the same as mutex ....
// ... ...

int wait_for_locker(locker& lock, unsigned wat_ms = INFINITE)
{
    assert(lock.is_valid());
    return ::WaitForSingleObject(lock.handle(), wat_ms);
}

int wait_for_locker(vector<locker*> locks, bool is_wait_all, unsigned wat_ms = INFINITE)
{
    if (locks.empty())
        return 0;

    vector<HANDLE> handles;
    handles.resize(locks.size());
    for (unsigned i=0; i < handles.size(); ++i)
    {
        assert (locks[i]->is_valid());
        handles[i] = locks[i]->handle();
    }

    return ::WaitForMultipleObjects(handles.size(), handles.data(), is_wait_all, wat_ms);
}

/// -- create and destory --- ///
static inline locker* create_locker(const string_t& name, int num)
{
    locker* ret = NULL;
    if (name.empty())
    {
        if (0 == num)
            ret = new mutex;
        else if (1 == num)
            ret = new event;
        else if (2 == num)
            ret = new critical_section;
        else if (3 == num)
            ret = new semaphore;
        else if (4 == num)
            ret = new mutex2;
        // ...
    }
    else
    {
        switch (num)
        {
        case 0:
            ret = new mutex(name);
            break;
        case 1:
            ret = new event(name);
            break;
        case 2:
            ret = new critical_section(name);
            break;
        case 3:
            ret = new semaphore(name);
            break;
        case 4:
            ret = new mutex2(name);
            break;
            // ...
        default:
            ;//error ...
        }
    }
    return ret;
}

static inline void destory_locker(locker*& lock_ptr)
{
    if (lock_ptr)
    {
        delete lock_ptr;
        lock_ptr = NULL;
    }
}

/// -- test --- ///

#ifdef _DEBUG
void test_win32_sync_func(int num = 0);
#endif
}/// namespace junkun

#endif // WIN32_SYNC_FUNC_H_INCLUDED
