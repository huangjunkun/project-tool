//---------------------------------------------------------------------------

#ifndef _BASE64_H_
#define _BASE64_H_
//---------------------------------------------------------------------------
#include <iostream>
#include <algorithm>
#include <string>
#include <strstream>
#include<sstream>
using namespace std;

namespace base64
{
typedef std::string string_t;
typedef std::istream istream_t;
typedef std::ostream ostream_t;
typedef std::istringstream istringstream_t;
typedef std::ostringstream ostringstream_t;
typedef char char_t;
typedef unsigned char Byte;

//---------------------------------------------------------------------------
typedef struct MyChar
{
    char_t	ch;
    bool    used;
}   MyChar;

//---------------------------------------------------------------------------
/// static
void encode_stream(istream_t* pInStream, ostream_t* pOutStream, string_t key="");
void decode_stream(istream_t* pInStream, ostream_t *pOutStream, string_t Key="");
string_t  base64_encrypt(const string_t& inStr);
string_t  base64_decrypt(const string_t& inStr);
string_t  rand_key4base64();

}
#endif //_BASE64_H_
