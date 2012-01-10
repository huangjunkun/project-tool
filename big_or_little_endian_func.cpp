#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <exception>
#include <process.h>

using namespace std;

 
/**
** 源文件：big_or_little_endian_func.cpp
** 功能描述：提供字节序转化的基本函数，包括判断本机字节序高端或低端功能。
** ** 包含测试程序。
** 
** 作者：junkun huang  e-mail：huangjunkun@gmail.com
** 日期：2010-11-25 /
*/


namespace test_namespace
{
    static int Hash(const char *z, int n)
    {
        int h = 0;
        if( n<=0 )
        {
            n = strlen(z);
        }
        while( n-- )
        {
            h = h ^ (h<<5) ^ *z++;
        }
        if( h<0 ) h = -h;
        return h;
    }


    template <typename T>
    class test_t
    {
    public:
        static T s_value;
    };

    template <typename T>
    T test_t<T>::s_value = 0;
}

namespace test_namespace
{

bool _TestBigEndian()
{
    union UnionForTest
    {
        short _short;
        char  _char;
    };
    UnionForTest test;
    test._short = 0x0100;
    return 1 == (int)test._char;
}
bool TestBigEndian()
{
    static bool s_is_big_endian = _TestBigEndian();
    return s_is_big_endian;
}

bool _TestLittleEndian()
{
    union UnionForTest
    {
        short _short;
        char  _char;
    };
    UnionForTest test;
    test._short = 0x0001;
    return 1 == (int)test._char;
}
bool TestLittleEndian()
{
    static bool s_is_little_endian = _TestLittleEndian();
    return s_is_little_endian;
}

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned __int64    uint64;

uint16 htons(uint16 n)
{
    if (TestBigEndian())
        return n;

    return	((n & 0xff) << 8) |
            ((n & 0xff00) >> 8);
}
uint16 ntohs(uint16 n)
{
    return htons(n);
}

uint32 htonl(uint32 n)
{
    if (TestBigEndian())
        return n;

    return ((n & 0xff) << 24) |
           ((n & 0xff00) << 8) |
           ((n & 0xff0000) >> 8) |
           ((n & 0xff000000) >> 24);
}
uint32 ntohl(uint32 n)
{
    return htonl(n);
}

uint64 htonll(uint64 n)
{
    if (TestBigEndian())
        return n;

    return ((n & 0xff) << 56) |
           ((n & 0xff00) << 40) |
           ((n & 0xff0000) << 24) |
           ((n & 0xff000000) << 8) |
           ((n & 0xff00000000LL) >> 8) |
           ((n & 0xff0000000000LL) >> 24) |
           ((n & 0xff000000000000LL) >> 40) |
           ((n & 0xff00000000000000LL) >> 56);
}
uint64 ntohll(uint64 n)
{
    return htonll(n);
}

uint8  hton(uint8 n)
{
    return n;
}
uint16 hton(uint16 n)
{
    return htons(n);
}
uint32 hton(uint32 n)
{
    return htonl(n);
}
uint64 hton(uint64 n)
{
    return htonll(n);
}

uint8  ntoh(uint8 n)
{
    return n;
}
uint16 ntoh(uint16 n)
{
    return ntohs(n);
}
uint32 ntoh(uint32 n)
{
    return ntohl(n);
}
uint64 ntoh(uint64 n)
{
    return ntohll(n);
}

} // namespace test_namespace

int main()
{

    ///*
        {
        using namespace test_namespace;
        uint8  a_uint8  = 0x12;
        uint16 a_uint16 = 0x1234;
        uint32 a_uint32 = 0x12345678;
        uint64 a_uint64 = 0x1234567890abcdef;
        std::cout << " " << TestBigEndian() << "\n";

        std::cout << hex << a_uint8 << "\n";
        std::cout << hex << a_uint16 << "\n";
        std::cout << hex << a_uint32 << "\n";
        std::cout << hex << a_uint64 << "\n";

        std::cout << hex << hton(a_uint8) << "\n";
        std::cout << hex << hton(a_uint16) << "\n";
        std::cout << hex << hton(a_uint32) << "\n";
        std::cout << hex << hton(a_uint64) << "\n";

        }
    /*
        {
        using namespace TestSpace;
        cout << " test_big_endian:" << test_big_endian() << " ,test_little_endian:" << test_little_endian() << "\n";
        cout << " test_big_endian:" << test_big_endian() << " ,test_little_endian:" << test_little_endian() << "\n";
        cout << " test_big_endian:" << test_big_endian() << " ,test_little_endian:" << test_little_endian() << "\n";

        }

   /** =========================*/
    cout << "Hello world!" << endl;
    return 0;
}
