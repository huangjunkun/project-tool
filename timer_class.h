#ifndef TIMER_CLASS_H_INCLUDED
#define TIMER_CLASS_H_INCLUDED

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
// #include <windows.h>
// #include <process.h>

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

struct timer_item
{
	bool is_op_timer;	
	unsigned requester_addr;	
	unsigned timer_type;
	unsigned remain_millisecond;

	//内部实现需要
	bool	is_cycled;
	unsigned period;
	unsigned next_expired_time;
};

class timer_handler  
{
public:
	timer_handler();
	virtual ~timer_handler();
	virtual void handle_timeout( unsigned timer_type ) = 0;

};

class timer_manager
{
public:
	timer_manager();
	virtual ~timer_manager();
	bool add_item( timer_handler* handler_ptr,  unsigned millisecond,
		unsigned timer_type ,bool is_cycled = false);
	void remove_item( timer_handler* handler_ptr);

// 	bool add_item( asyn_io_operation* operation_ptr, unsigned millisecond );
// 	void remove_item( asyn_io_operation* operation_ptr );

	bool has_timer_item();
	void get_latest_item( timer_item& result );
	void pop_front();
};

class timer
{
public:
    typedef timer self_type;
    static const unsigned DEFAULT_INTERVAL_TICK = 1000;

public:
	virtual void handle_timeout(unsigned timer_type) = 0;

};


}/// namespace test_namespace


#endif // TIMER_CLASS_H_INCLUDED
