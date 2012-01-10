#ifndef THREAD_CLASS_H_INCLUDED
#define THREAD_CLASS_H_INCLUDED

#include <iostream>
#include <cstdio>
#include <cassert>

#include <iomanip>
#include <limits.h>
#include <string>
#include <vector>

//#ifndef _VC
//#include <auto_ptr.h>///non-VC
//#endif

#include <memory>

#include <map>
#include <iterator>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <strstream>
//#include <alloca.h>
#include <windows.h>
#include <process.h>

/**
** 源文件： thread_class.h, thrad_class_test.cpp
** 功能描述：封装WIN线程API，抽象实现线程基类thread_base。
** 方便用于定制具体操作的线程子类。
**
** 作者：junkun huang  e-mail：huangjunkun@gmail.com
** 日期：2010-12-25 /
*/
///
namespace test_namespace
{

#pragma warning(disable : 4290)

unsigned __stdcall lanuch_thread_proc(void* args);

template <class T>
class thread_launcher_t
{
public:
    thread_launcher_t()
        : _pObj(0), _pArgs(0)
    {}
    thread_launcher_t(T* pObj)
        : _pObj(pObj), _pArgs(0)
    {}
    thread_launcher_t(void* pArgs)
        : _pObj(0), _pArgs(pArgs)
    {}
    thread_launcher_t(T* pObj, void* pArgs)
        : _pObj(pObj), _pArgs(pArgs)
    {}
    void set_launcher(T* pObj, void* pArgs)
    {
        _pObj = pObj;
        _pArgs = pArgs;
    }
    void set_args(void* pArgs)
    {
        _pArgs = pArgs;
    }
    void set_obj(T* pObj)
    {
        _pObj = pObj;
    }

    unsigned launch()
    {
        assert(_pObj);// && _pArgs
        return _pObj->thread_proc(_pArgs);
    }
private:
    T*      _pObj;
    void*   _pArgs;
};

class thread_base;
typedef thread_launcher_t<thread_base> thread_launcher;

class thread_base
{
public:
    typedef unsigned ( *start_address )( void * );

    static void* operator new(std::size_t size) throw(std::bad_alloc)
    {
        std::cout << " thread_base::operator new(std::size_t size) throw(std::bad_alloc) ...\n";
        return ::operator new(size);
    }
    static void operator delete(void * mem) throw()
    {
        std::cout << " thread_base::operator delete(void * mem) throw() ...\n";
        ::operator delete(mem);
    }

    static void* operator new(std::size_t size, const std::nothrow_t& nt) throw()
    {
        std::cout << " thread_base::operator new(std::size_t size, const std::nothrow_t& nt) throw() ...\n";
        return ::operator new(size, nt);
    }
    static void operator delete(void * mem, const std::nothrow_t& nt) throw()
    {
        std::cout << " thread_base::operator delete(void * mem, const std::nothrow_t& nt) throw()...\n";
        return ::operator delete (mem);
    }
protected:
    // 不允许静态创建，可以 new ???
    thread_base(void* args, bool is_create_suspended=false)
        : _handle_thread(0), _thread_id(0), _launcher(args) //, _launcher(this, args)
    {
        _do_constor(is_create_suspended);
    }
    virtual ~thread_base()
    {
        if (_handle_thread)
        {
            //_endthreadex(0);
            CloseHandle(_handle_thread);
        }
    }
public:
    HANDLE handle() const
    {
        return _handle_thread;
    }
    unsigned thread_id() const
    {
        return _thread_id;
    }

    int resume()
    {
        assert(_handle_thread);
        return ::ResumeThread(_handle_thread);
    }
    int suspend()
    {
        assert(_handle_thread);
        ::SuspendThread(_handle_thread);
    }
    //////////////////////////////////////////////////////////////////////////
    virtual unsigned thread_proc(void* args) = 0;
private:
    void _do_constor(bool is_create_suspended)
    {
        unsigned init_flag;
        if (is_create_suspended)
            init_flag = CREATE_SUSPENDED;
        else
            init_flag = 0;
        _launcher.set_obj(this);
        _handle_thread = (HANDLE)_beginthreadex(NULL, 0, &thread_base::_lanuch_thread_proc, (void*)&_launcher, init_flag, &_thread_id);//__stdcall
        assert(_handle_thread);
    }

    static unsigned __stdcall _lanuch_thread_proc(void* args);

protected:
    HANDLE		_handle_thread;
    unsigned	_thread_id;
    thread_launcher    _launcher;
};
// static member function ...
unsigned  __stdcall thread_base::_lanuch_thread_proc(void* args)
{
    thread_launcher* pObj = static_cast<thread_launcher*> (args);
    return pObj->launch();
    _endthreadex(0);
}

unsigned thread_base::thread_proc(void* args)
{
    std::cout << " unsigned thread_base::thread_proc(void* args) ...\n";
    return 0;
}

// global function ...
unsigned __stdcall lanuch_thread_proc(void* args)
{
    thread_launcher* pObj = static_cast<thread_launcher*> (args);
    return pObj->launch();
    _endthreadex(0);
}
//////////////////////////////////////////////////////////////////////////

template <bool IsCreateSuspended>
class test_thread_t : public thread_base
{
public:
    static void* operator new(std::size_t size) throw(std::bad_alloc)
    {
        std::cout << " test_thread_t::operator new(std::size_t size) throw(std::bad_alloc)...\n";
        return ::operator new(size);
    }
    static void operator delete(void * mem) throw()
    {
        std::cout << " test_thread_t<IsCreateSuspended>::operator delete(void* mem, size_t size) throw()...\n";
        ::operator delete (mem);
    }

    static void* operator new(std::size_t size, const std::nothrow_t& nt) throw()
    {
        std::cout << " operator test_thread_t::new(std::size_t size, const std::nothrow_t& nt) throw()...\n";
        return ::operator new(size, nt);
    }
    static void operator delete(void * mem, const std::nothrow_t& nt) throw()
    {
        std::cout << " operator test_thread_t::delete(void * mem, const std::nothrow_t& nt) throw()...\n";
        return ::operator delete (mem);
    }

    static void* operator new[](std::size_t size) throw(std::bad_alloc);
    static void operator delete[](void * mem);

    static void* operator new(std::size_t size, void *ptr) throw();
    static void operator delete(void * mem, void *ptr) throw();
    // 	static void* operator new[](std::size_t size, void *ptr) throw();
    // 	static void operator delete[](void * mem, void *ptr);
public:
    test_thread_t() : thread_base(NULL, IsCreateSuspended)
    { std::cout << "test_thread_t()" << IsCreateSuspended << "\n"; }
    test_thread_t(void* args)
        : thread_base(args, IsCreateSuspended)
    { std::cout << "test_thread_t(void* args)" << IsCreateSuspended << "\n"; }
//    test_thread_t(void* args, bool is_create_suspended)
//        : thread_base(args, is_create_suspended)
//    { std::cout << "test_thread_t(void* args, bool is_create_suspended)" << is_create_suspended << "\n"; }
    virtual ~test_thread_t() { std::cout << "~test_thread_t()" << IsCreateSuspended << "\n"; }

    virtual unsigned thread_proc(void* args)
    {
        unsigned count_sum = 5;
        if (0 != args)
            count_sum = *static_cast<unsigned*>(args);

        unsigned tick_count = 0;
        while (tick_count < count_sum)
        {
            std::cout << _thread_id << "(thread_id), ... tick count(1 tick/s), tick_count -> " << ++tick_count << "\n";
            Sleep(1000);
        }
        return 0;
    }
};

typedef test_thread_t<true> test_thread;
typedef test_thread_t<false> test_thread2;


template <bool IsCreateSuspended>
void* test_thread_t<IsCreateSuspended>::operator new[](std::size_t size) throw(std::bad_alloc)
{
    std::cout << " test_thread_t<IsCreateSuspended>::operator new(std::size_t size) throw(std::bad_alloc)...\n";
    std::cout << " size: " << size << "\n";
    return ::operator new(size) ;
}

template <bool IsCreateSuspended>
void test_thread_t<IsCreateSuspended>::operator delete[](void* mem)
{
    std::cout << " test_thread_t<IsCreateSuspended>::operator delete[](void* mem, size_t size)...\n";
    ::operator delete[] (mem);
}

template <bool IsCreateSuspended>
void* test_thread_t<IsCreateSuspended>::operator new(std::size_t size, void *ptr) throw()
{
    std::cout << " test_thread_t<IsCreateSuspended>::operator new(std::size_t size, void *ptr)...\n";
    return ::new test_thread_t<IsCreateSuspended>(ptr);
}

template <bool IsCreateSuspended>
void test_thread_t<IsCreateSuspended>::operator delete( void* mem, void *ptr) throw()
{
    return ::operator delete(mem, ptr);
}

// template <bool IsCreateSuspended>
// void* test_thread_t<IsCreateSuspended>::operator new[](std::size_t size, void *ptr) throw()
// {
// 	std::cout << " test_thread_t<IsCreateSuspended>::operator new[](std::size_t size, void *ptr)...\n";
// 	return ::new test_thread_t[size](ptr);
// }
// template <bool IsCreateSuspended>
// void test_thread_t<IsCreateSuspended>::operator delete[]( void* mem, void *ptr )
// {
// 	return ::operator delete(mem, ptr);
// }

//delete void *
//这一操作对于一些简单的系统内建类型不会有问题，但如果void*所指向的对象在析构函数里要释放，
//进行此造作会丢失内存，因为它不执行析构函数，例如：

}/// namespace test_namespace


#endif // THREAD_CLASS_H_INCLUDED
