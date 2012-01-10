#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

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

namespace test_namespace
{
/**
** 类型CCoutBufferT，实现输入输出的缓冲区，先进先出的缓冲区。如：
** [数据输出到缓冲区顺序] -> 1 -> 2 -> 3 | [缓冲区输入到数据顺序] 1 <- 2 <- 3
** 输入过程中，缓冲区会自动调整缓冲大小。目前仅支持类型unsigned，int，tchar，std::string，还有vector集合。
** 支持其他或基本类型自定义类型的输入输出，需设计者继续自行扩展。
** 作者：junkun huang  e-mail：huangjunkun@gmail.com
** 日期：2010-11-25 /
*/
    template <typename TChar>
    class CCoutBufferT
    {
    public:
        CCoutBufferT()
        : _buffer(0), _buffer_size(0), _write_pos(0), _read_pos(0)
        {}
        ~CCoutBufferT()
        {
            _safe_delete_buffer();
        }

        void reset()
        {
            //memset(_buffer, '\0', _buffer_size);
            _read_pos = _write_pos = 0;
        }
        unsigned length() const
        {
            assert(_write_pos >= _read_pos);
            return (_write_pos-_read_pos);
        }
        unsigned write_pos() const { return _write_pos; }
        unsigned read_pos() const { return _read_pos; }
        unsigned capability() const { return _buffer_size; }
        const TChar* buffer() const
        {
            return _buffer;
        }

        TChar* buffer()
        {
            return const_cast<TChar*>(buffer());
        }
        std::string to_string() const { return std::string(_buffer+_read_pos, _buffer+_write_pos);}

        CCoutBufferT& operator >> (int& obj)
        {
            unsigned length = sizeof(obj)/sizeof(TChar);
            if (this->length() >= length)
            {
                memcpy(&obj,_buffer+_read_pos , sizeof(obj));
                _read_pos += length;
            }
            //else { throw std::bad_exception(); }//or throw as error ...

            return *this;
        }

        CCoutBufferT& operator << (int obj) throw (std::bad_exception)
        {
            unsigned length = sizeof(obj)/sizeof(TChar);
            bool apply_res = _try_apply_buffer(length);

            assert(apply_res);//or throw as error ...
            if (!apply_res)
                throw std::bad_exception();

            memcpy(_buffer+_write_pos, &obj, sizeof(obj));
            _write_pos += length;
            return *this;
        }

        CCoutBufferT& operator >> (unsigned& obj)
        { return operator >>( (int&)obj ); }

        CCoutBufferT& operator << (unsigned obj)
        { return operator <<( (int)obj ); }

        CCoutBufferT& operator >> (std::basic_string<TChar>& obj) throw (std::bad_exception)
        {
            obj.clear();
            unsigned size;
            operator >> (size);/// step 1
            assert(size <= length());
            if (size > length())
                throw std::bad_exception();

            TChar* read_pos = _buffer+_read_pos;
            std::copy(read_pos, read_pos+size, back_inserter(obj));/// step 2
            assert(size == obj.size());
            _read_pos += obj.size();

            return *this;
        }

        CCoutBufferT& operator << (const std::basic_string<TChar>& obj) throw (std::bad_exception)
        {
            unsigned length = sizeof(obj.size())/sizeof(TChar) + obj.size();
            bool apply_res = _try_apply_buffer(length);
            assert(apply_res);//or throw as error ...
            if (!apply_res)
                throw std::bad_exception();

            operator << (obj.size());/// step 1
//            unsigned copy_size = obj.size()*sizeof(TChar);
//            memcpy(_buffer+_write_pos, obj.c_str(), copy_size);/// step 2
            std::copy(obj.begin(), obj.end(), _buffer+_write_pos);
            _write_pos += obj.size();

            return *this;
        }

        template <typename T>
        CCoutBufferT& operator >> (std::vector<T>& obj)
        {
            unsigned size;
            *this >> size;
            for (size_t i=0; i < size; i++)
            {
                T   temp;
                *this >> temp;
                obj.push_back(temp);
            }

            return *this;
        }

        template <typename T>
        CCoutBufferT& operator << (const std::vector<T>& obj)
        {
            *this << obj.size();
            for (size_t i=0; i < obj.size(); i++)
                *this << obj[i];

            return *this;
        }

        TChar* reserve(unsigned size)
        {
            if (_buffer_size < size)
            {
                TChar* buffer = new TChar[size];
                if (!buffer)
                    return NULL;//new error, or throw...

                memcpy(buffer, _buffer, _write_pos*sizeof(TChar));
                _safe_delete_buffer();

                _buffer = buffer;
                _buffer_size = size;

            }
            return _buffer;
        }

    protected:
        void _safe_delete_buffer()
        {
            if (_buffer)
            {
                delete _buffer;
                _buffer = NULL;
            }
        }

        bool _try_apply_buffer(unsigned length)
        {
            if (_write_pos+length > _buffer_size)
            {
                TChar* buffer = new TChar[2*(_write_pos+length)+1];
                if (!buffer)
                    return false;//new error ...

                memcpy(buffer, _buffer, _write_pos*sizeof(TChar));
                _safe_delete_buffer();

                _buffer = buffer;
                _buffer_size = 2*(_write_pos+length);
                //memset(_buffer, '\0', _buffer_size);
            }
            return true;
        }

        void _abjust_buffer() throw (std::bad_exception)
        {
            if (_read_pos == _write_pos)
                reset();
            else if (_read_pos < _write_pos)
            {
                if (_read_pos > 0)
                {
                    memcpy(_buffer, _buffer+_read_pos, length()*sizeof(TChar));
                }
            }
            else // _read_pos > _write_pos
            {
                assert(false);
                throw std::bad_exception();
            }
        }

    private:
        TChar*      _buffer;
        unsigned    _buffer_size;
        unsigned    _write_pos;
        unsigned    _read_pos;
    };

    typedef CCoutBufferT<char> CCoutBuffer;
    typedef CCoutBufferT<wchar_t> CCoutBufferW;

    static CCoutBuffer cout_buffer;
    static CCoutBufferW wcout_buffer;
////////////////////////////////////////////////////////////////

    class CTestCoutBuffer
    {
    public:
        CTestCoutBuffer() : _value("CTestCoutBuffer::_value \n") {}
        const std::string& value() const { return _value; }
        void setValue(const std::string& value) { _value = value; }

        friend CCoutBuffer& operator << (CCoutBuffer& outs, const CTestCoutBuffer& obj);//const
        friend CCoutBuffer& operator >> (CCoutBuffer& ins, CTestCoutBuffer& obj);
    private:
//    public:
        std::string _value;
    };

    CCoutBuffer& operator << (CCoutBuffer& outs, const CTestCoutBuffer& obj)
    {
        outs << obj._value;
        return outs;
    }
    CCoutBuffer& operator >> (CCoutBuffer& ins, CTestCoutBuffer& obj)
    {
        ins >> obj._value;
        return ins;
    }

}/// namespace test_namespace

#endif // MAIN_H_INCLUDED
