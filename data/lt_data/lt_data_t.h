#ifndef LT_DATA_LT_DATA_T_H
#define LT_DATA_LT_DATA_T_H

#include <cstring>
#include <assert.h>
#include <string>
#include <vector>

//#include "../log/include/awe_log.h"
class lt_data_t
{
public:
    unsigned long _length;
private:
    unsigned char *_buf_from_outside;
    unsigned char *_buf_self_generated;
    void *private_data;
    std::vector<void *> data_stack;

public:
    lt_data_t() : _buf_from_outside(NULL), _buf_self_generated(NULL), _length(0)
    {
    }

    lt_data_t(unsigned long length) : _buf_from_outside(NULL), _buf_self_generated(NULL), _length(length)
    {
        realloc_buf();
    }

    lt_data_t(unsigned long length, unsigned char *buf) : _buf_from_outside(buf), _buf_self_generated(NULL),
                                                          _length(length)
    {
    }

    lt_data_t(const lt_data_t &other):_buf_from_outside(nullptr), _buf_self_generated(NULL),
    _length(other._length)
    {
        if(_length == 0)
        {
            return ;
        }
        if ( other.is_buf_from_outside())
        {
            _buf_from_outside = other._buf_from_outside;
            _buf_self_generated = NULL;
            _length = other._length;
        }
        else
        {
            _length = other._length;
            realloc_buf();
            memcpy(_buf_from_outside, other._buf_from_outside, sizeof(_length) + _length);
        }
    }

    lt_data_t &operator=(const lt_data_t &other)
    {
        if ( other.is_buf_from_outside())
        {
            _buf_from_outside = other._buf_from_outside;
            _buf_self_generated = NULL;
            _length = other._length;
        }
        else
        {
            _length = other._length;
        
            realloc_buf();
            memcpy(_buf_from_outside, other._buf_from_outside, sizeof(_length) + _length);
            
            mark_buf_self_generated();
        }
        return *this;
    }

    unsigned char *get_buf() const
    {
        if ( !is_buf_from_outside())
            return &_buf_self_generated[sizeof(_length)];
        else
            return _buf_from_outside;
    }

    unsigned char *get_data()
    {
        assert(!is_buf_from_outside());
        return _buf_self_generated;
    }

    unsigned long data_len()
    {
        assert(!is_buf_from_outside());
        return _length + sizeof(_length);
    }

    void realloc_buf()
    {
        /*
        if (is_buf_from_outside()) {
            AWE_MODULE_INFO("lt_data_t--", "_buf_self_generated %u,  "
                                           "_buf_form_outside %u, ", _buf_self_generated, _buf_from_outside);
        }
         */
        assert(!is_buf_from_outside());
        free_buf();
        /*
        if(_length > 1024 * 1024){
            AWE_MODULE_INFO("lt_data_t--", "length : %u", _length);
        }
         */
        _buf_self_generated =
                static_cast<unsigned char *>(malloc(_length + sizeof(_length)));
        /*
        if (!_buf_self_generated) {
            AWE_MODULE_INFO("lt_data_t--", "length : %u", _length);
        }
         */
        assert(_buf_self_generated);
        std::memcpy(_buf_self_generated, &_length, sizeof(_length));
        mark_buf_self_generated();
    }
    
    std::string to_less_string()
    {
        std::string res= "total_cnt[" +std::to_string(this->_length) + "],buf[";
        auto first_size = (8>this->_length)?this->_length:8;
        auto sencod_size = (16 > this->_length)?this->_length - 16:8;
        unsigned char *buf = this->get_buf();
        for(int i = 0;i < first_size; ++i )
        {
            char tmp_buf[32] = {0};
            sprintf(tmp_buf, "%02x ", buf[i]);
            res += std::string(tmp_buf);
        }
        if(first_size < 8) return res + "]";
        res += "... ";
        for(int i = this->_length - sencod_size;i < this->_length; ++i )
        {
            char tmp_buf[32] = {0};
            sprintf(tmp_buf, "%02x ", buf[i]);
            res += std::string(tmp_buf);
        }
        return res + "]";
    }
    
    std::string to_string()
    {
        std::string result = "len : " + std::to_string(this->_length) + "\n";

        if ( _length == 0 )
        {
            return result;
        }

        result += "buf : \n";
        unsigned char *buf = this->get_buf();
        unsigned long i = 0;

        if ( this->_length <= 1024 )
        {
            for ( i = 0; i < this->_length; ++i )
            {
                if ( i % 32 == 0 )
                {
                    char tmp[32] = {0};
                    sprintf(tmp, "%-10d ", i);
                    result += std::string(tmp);
                }
                char tmp_buf[32] = {0};
                sprintf(tmp_buf, "%02x ", buf[i]);
                result += std::string(tmp_buf);
                if ( i % 32 == 31 )
                {
                    result += "\n";
                }
            }
        }
        else
        {
            for ( i = 0; i < 512; ++i )
            {
                if ( i % 16 == 0 )
                {
                    char tmp[32] = {0};
                    sprintf(tmp, "%-10d ", i);
                    result += std::string(tmp);
                }
                char tmp_buf[32] = {0};
                sprintf(tmp_buf, "%02x ", buf[i]);
                result += std::string(tmp_buf);
                if ( i % 16 == 15 )
                {
                    result += "\n";
                }
            }

            result += std::string("...\n");
            result += std::string("\n");
            result += std::string("...\n");

            unsigned long start = this->_length%512;
            i = start == 0 ? this->_length-512 : this->_length-start-512;

            for ( ; i < this->_length; ++i )
            {
                if ( i % 16 == 0 )
                {
                    char tmp[32] = {0};
                    sprintf(tmp, "%-10d ", i);
                    result += std::string(tmp);
                }
                char tmp_buf[32] = {0};
                sprintf(tmp_buf, "%02x ", buf[i]);
                result += std::string(tmp_buf);
                if ( i % 16 == 15 )
                {
                    result += "\n";
                }
            }
        }
        result += "\n";
        return result;
    }

    void push_private(void *new_private)
    {
        data_stack.push_back(private_data);
        private_data = new_private;
    }

    void *pop_private()
    {
        void *cur_private = private_data;
        if ( data_stack.size() != 0 )
        {
            private_data = data_stack.back();
            data_stack.pop_back();
        }
        return cur_private;
    }

    ~lt_data_t()
    {
        free_buf();
    }

private:
    bool is_buf_from_outside() const
    {
        return _buf_from_outside != _buf_self_generated;
    }

    void mark_buf_self_generated()
    {
        _buf_from_outside = _buf_self_generated;
    }

    void free_buf()
    {
        if ( _buf_self_generated )
        {
            free(_buf_self_generated);
            _buf_self_generated = nullptr;
        }
    }

};

#endif
