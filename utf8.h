#ifndef windsoul_utf8_h
#define windsoul_utf8_h
// copy from cloudwu's windsoul
// 2011-12-13/ huangjunkun /

#include <stddef.h>

typedef unsigned short w_char;

size_t utf8ToWideString(const char * utf8, w_char * utf16, size_t sz);
size_t utf8FromWideString(const w_char * utf16, char * utf8, size_t sz);

#endif
