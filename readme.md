/**
** project name:  project-tool
** date:          2011-12-19 14:45
** author:        junkun huang
** e-mail:        huangjunkun@gmail.com
** Declare: 
  计划在该git工程存放，工程性的工具代码。欢迎交流，谢谢！
**/

**/
10.


**/
10.


**/
10.


**/
10.

**/
## 10.
/**
** 源文件： TransByteOrderStream.h TransByteOrderStream.c
** 功能说明： 用于传输的字节流编解码，区分字节的网络序与主机序。
** 类型TransByteOrderIOStream的详细功能已参加代码。
** 开发+测试环境：VS2003 & CodeBlocks10.02
** 作者：junkun huang
** 日期：2010-12-02 /
*/

**/
## 9.
/**
** 源文件： utf8.h utf8.c
** 功能说明： 字符编码转换功能，包括utf8转宽字节，宽字节转utf8。引用自cloudwu's github form
```C
** https://github.com/cloudwu/windsoul/blob/master/src/core/utf8.c
** https://github.com/cloudwu/windsoul/blob/master/src/core/utf8.h
```
*/

## 8.
/**
** 源文件： win_file.h, win_file.cpp, win_file_test.cpp
** 功能说明：
** 类型win_file封装Win32-API文件操作，完成一组基本的文件操作功能包含：
** #1 win_file对象自动创建或打开，关闭文件。文件读写，设置位置。
** #2 静态方法有创建目录，删除目录，判断目录，删除文件，复制文件，文件重命名。
** #3 注意，在文件操作返回失败时，建议调用error()或err_msg()获取具体的错误信息.
** #4：操作系统：windows 7 专业版
** 开发+测试环境：VS2003 + CodeBlocks10.02
** 作者：junkun huang
** 日期：2011-09-02 /
*/

## 7. 
/**
** 源文件： win32_sync_func.h, win32_sync_func_test.cpp
** 功能说明：
** #1：先抽象基类型locker，然后派生不同子类型mutex，event，critical_section，semaphore四款常用的锁，均实用于需要同步操作情况。
** 注意：其中类型critical_section非WIN内核对象区别于其他类型内核对象，其包含成员数据稍微不同。
** #2：包含另一种锁实现模式，模板推导技术完成，即win_sync_functor，同样继承基类locker。需根据不同锁操作来定义不同相关类型：
** _Constructor, _Destoryer, _Locker, _Unlocker，该类型用于win_sync_functor的模板参数，最终完成类型的推导。
** 已实现推导类型mutex2（可类似设计类型event2，critical_section2，semaphore2）。
** #3：模板类型scoped_lock_t用于辅助以上不同锁类型的使用。以上具体实现细节详见代码。
** #4：操作系统：windows 7 专业版
** 	开发+测试环境：VS2003 + CodeBlocks10.02

** 作者：junkun huang
** 日期：2011-10-16 /
*/

## 6. 
/**
** 源文件： SqliteOperator.h, SqliteOperator.cpp, SqliteOperatorTest.cpp
** 功能说明：
** 封装sqlite原生的数据库操作API。可以简便完成执行SQL语句，操作包括创建，插入，更新，删除，查询数据等。
** 详细见下面类型CSqliteOperator的定义。还定义其他的sqlite辅助类型，包括数据库类型，事务类型，锁类型。

** 作者：junkun huang
** 日期：2011-12-02 /
*/


## 5.

/**
** 源文件： win32_timer_class.h, win32_timer_class_test.cpp
** 封装WIN定时器操作API，抽象实现timer类型，方便使用。
**
** 作者：junkun huang
** 日期：2011-06-25 /
*/

## 4.

/**
** 源文件： win32_thread_class.h, win32_thrad_class_test.cpp
** 功能描述：封装WIN线程API，抽象实现线程基类thread_base。
** 方便用于定制具体操作的线程子类。
**
** 作者：junkun huang
** 日期：2010-12-25 /
*/

## 3.

/**
** 文件： c_singe_file.h，c_singe_file_test.cpp
**
** 函数： trim_tstring
** 功能： 裁剪字符串，针对字符串首尾裁剪空格与制表符。
** 类型： c_single_file
** 功能： 封装CRT基本的文件读写操作。方便使用。
** 作者：junkun huang
** 日期：2011-07-21 /
*/

## 2.
/**
** 源文件：cout_buffer.h, cout_buffer_test.cpp
** 功能描述：
** 类型CCoutBufferT，实现输入输出的缓冲区，先进先出的缓冲区。如：
** [数据输出到缓冲区顺序] -> 1 -> 2 -> 3 | [缓冲区输入到数据顺序] 1 <- 2 <- 3
** 输入过程中，缓冲区会自动调整缓冲大小。目前仅支持类型unsigned，int，tchar，std::string，还有vector集合。
** 支持其他或基本类型自定义类型的输入输出，需设计者继续自行扩展。

** 作者：junkun huang
** 日期：2010-11-25 /
*/

## 1. 
/**
** 源文件：big_or_little_endian_func.cpp
** 功能描述：提供字节序转化的基本函数，包括判断本机字节序高端或低端功能。
** ** 包含测试程序。
** 
** 作者：junkun huang
** 日期：2010-11-25 /
*/

### 作者：junkun huang  e-mail：huangjunkun@gmail.com