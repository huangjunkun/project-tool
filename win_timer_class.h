#ifndef WIN_TIMER_CLASS_H_INCLUDED
#define WIN_TIMER_CLASS_H_INCLUDED

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
** 源文件： timer_class.h, timer_class_test.cpp
** 封装WIN定时器操作API，抽象实现timer类型，方便使用。
**
** 作者：junkun huang  e-mail：huangjunkun@gmail.com
** 日期：2011-06-25 /
*/
///
namespace test_namespace
{

class win_timer
{
public:
    typedef win_timer self_type;
    static const unsigned DEFAULT_INTERVAL_TICK = 1000;

public:
    win_timer()
        : _hwnd(NULL), _interval_tick(DEFAULT_INTERVAL_TICK)//
    {
        _timer_id = 0;
        reset();
    }
    win_timer(unsigned tick_time)
        : _hwnd(NULL), _interval_tick(tick_time)
    {
        _timer_id = 0;
        reset();
    }
    win_timer(HWND hWnd )
        : _hwnd(hWnd), _interval_tick(DEFAULT_INTERVAL_TICK)
    {
        std::cout << " win_timer(HWND hWnd ) ...\n";
        _timer_id = (UINT_PTR)this;
        reset();
    }
    win_timer(HWND hWnd, unsigned tick_time)
        : _hwnd(hWnd), _interval_tick(tick_time)
    {
        _timer_id = (UINT_PTR)this;
        reset();
    }

    virtual ~win_timer()
    {
        BOOL res = ::KillTimer(NULL, _timer_id);
        assert(TRUE == res);
    }
    //virtual void timer_proc() = 0;

protected:
    static void __stdcall s_timer_proc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)//
    {
        std::cout << " s_timer_proc ...\n";
        win_timer* this_timer = (win_timer*) idEvent;
        assert (this_timer);
        this_timer->handle_timeout(idEvent);
        return;
    }
    void reset()
    {
        std::cout << " _timer_id:" << _timer_id;
        _timer_id = ::SetTimer(_hwnd, _timer_id, _interval_tick, s_timer_proc);//0
        std::cout << " _timer_id:" << _timer_id << "\n";
// 		if (NULL == hWnd)
// 			_timer_id = (UINT_PTR)this;
        if (!_timer_id)
        {
            int res = ::GetLastError();
            std::cout << " GetLastError:" << res << "\n";
        }
        //assert (_timer_id);
    }
public:
    unsigned timer_id() const
    {
        return (unsigned)this;
    }
    HWND	 link_handle() const
    {
        return _hwnd;
    }
	unsigned interval_tick() const
	{
		return _interval_tick;
	}
    virtual void handle_timeout(unsigned timer_type) = 0;

private:
    HWND	_hwnd;
    UINT_PTR _timer_id;
    unsigned _interval_tick;
};



}/// namespace test_namespace


#endif // WIN_TIMER_CLASS_H_INCLUDED
