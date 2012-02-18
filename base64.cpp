//---------------------------------------------------------------------------
#include <strstream>
#include<sstream>
#include <iostream>
#include <vector>
#include "base64.h"

using namespace std;


namespace base64
{
//---------------------------------------------------------------------------
Byte g_base64_table[65] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
                           'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
                           'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
                           'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                           'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
                           'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', '+', '-', '='
                          };

Byte index_from_table(const Byte& b);
//---------------------------------------------------------------------------

void encode_stream(istream_t* pInStream, ostream_t* pOutStream, string_t key )
{
    if(key != "")
    {
        for(size_t i=0; i<key.length()&& i<64; i++)
        {
            g_base64_table[i] = key[i];
        }
    }
    int  Count, iI, iO;
    Byte    InBuf[45];
    char_t	OutBuf[63];
    Byte    Temp;
    std::fill(OutBuf, OutBuf+62, NULL);
    do
    {
        pInStream->read((char_t*)InBuf, sizeof(InBuf));
        Count = pInStream->gcount();
        if(Count==0)
            break;
        iI = 0;
        iO = 0;
        while(iI < Count-2)
        {
            //编码第1个字节
            Temp = (InBuf[iI] >> 2);
            OutBuf[iO] = char_t(g_base64_table[Temp&0x3f]);
            //编码第2个字节
            Temp = (InBuf[iI] << 4) | (InBuf[iI+1] >> 4);
            OutBuf[iO+1] = char_t(g_base64_table[Temp&0x3f]);
            //编码第3个字节
            Temp = (InBuf[iI+1] << 2) | (InBuf[iI+2] >> 6);
            OutBuf[iO+2] = char_t(g_base64_table[Temp&0x3f]);
            //编码第4个字节
            Temp = (InBuf[iI+2] & 0x3f);
            OutBuf[iO+3] = char_t(g_base64_table[Temp]);
            iI += 3;
            iO += 4;
        }
        if(iI <= Count-1)
        {
            //ShowMessage(String(InBuf[I]) );
            Temp = (InBuf[iI] >> 2 );
            //ShowMessage(String(Temp) );
            OutBuf[iO] = char_t(g_base64_table[Temp&0x3f] );
            if(iI==Count-1)
            {
                //一个奇数字节
                Temp = (InBuf[iI] << 4)& 0x30;
                OutBuf[iO+1] = char_t(g_base64_table[Temp&0x3f] );
                OutBuf[iO+2] = '=';
            }
            else //I==Count-2
            {
                //两个奇数字节
                Temp = ((InBuf[iI] << 4)& 0x30) | ((InBuf[iI+1] >> 4)& 0x0f);
                OutBuf[iO+1] = char_t(g_base64_table[Temp&0x3f] );
                Temp = (InBuf[iI+1] << 2) & 0x3c;
                OutBuf[iO+2] = char_t(g_base64_table[Temp&0x3f] );
            }
            OutBuf[iO+3] = '=';
            iO += 4;
        }
        pOutStream->write(OutBuf, iO);
    }
    while(!(Count < sizeof(InBuf)) );
}
//---------------------------------------------------------------------------
Byte index_from_table(const Byte&   b)
{
    for(Byte i=0; i < 65; ++i)
    {
        if(g_base64_table[i]==b)
            return i;
    }
    return Byte(255);
}
//---------------------------------------------------------------------------
void decode_stream(istream_t* pInStream, ostream_t* pOutStream, string_t key )
{

    if(key!="")
    {
        for(size_t i=0; i < key.length()&& i<64; ++i)
        {
            g_base64_table[i] = key[i];
        }
    }
    int     Count, iI, iO ;  //, c1, c2, c3
    Byte     c1, c2, c3;  //
    Byte    InBuf[88];
    Byte    OutBuf[66];
    do
    {
        pInStream->read((char_t*)InBuf, sizeof(InBuf));
        Count = pInStream->gcount();
        if(Count==0)
            break;
        iI = 0;
        iO = 0;
        while(iI<Count)
        {
            c1 = index_from_table(InBuf[iI]);
            c2 = index_from_table(InBuf[iI+1]);
            c3 = index_from_table(InBuf[iI+2]);
            if( c1==Byte(255)||c2==Byte(255)||c3==Byte(255))
            {
                string_t errStr = "非法字符 ，解密终止！";
                pOutStream->write(errStr.c_str(), errStr.length());
                return;   //throw; //???
            }
            OutBuf[iO] = ((c1<<2)|(c2>>4));
            iO++;
            if(char_t(InBuf[iI+2]!='=') )
            {
                OutBuf[iO] = ((c2<<4)|(c3>>2));
                iO++;
                if(char_t(InBuf[iI+3]!='='))
                {
                    OutBuf[iO] = ((c3<<6)|index_from_table(InBuf[iI+3]));
                    iO++;
                }
            }
            iI += 4;
        }

        pOutStream->write((char_t*)OutBuf, iO);
    }
    while(!(Count < sizeof(InBuf)));

}
//---------------------------------------------------------------------------
string_t base64_encrypt(const string_t& inStr)
{

    istringstream_t   inStream(inStr);
    ostringstream_t   outStream;
    encode_stream(&inStream, &outStream);

    return outStream.str();
}
//---------------------------------------------------------------------------
string_t base64_decrypt(const string_t& inStr)
{
    istringstream_t   inStream(inStr);
    ostringstream_t   outStream;
    decode_stream(&inStream, &outStream);

    return outStream.str();
}
//---------------------------------------------------------------------------
string_t rand_key4base64()
{
    /*
      time_t  t;
      srand((unsigned int)time(&t) );     */
    vector<MyChar>  mychar_vec(256);
    for(int i=0; i<256; i++)
    {
        mychar_vec[i].ch = (char_t)i;
        mychar_vec[i].used = false;
    }
    string_t Key;
    Key.resize(64);
    int     index = 0;
    while( index <64 )
    {
        int     i = rand()%256;
        if(mychar_vec[i].used==false)
        {
            Key[index++] = mychar_vec[i].ch;
            mychar_vec[i].used = true;
        }
    }
    return Key;
}
//---------------------------------------------------------------------------
} /// namespace base64

