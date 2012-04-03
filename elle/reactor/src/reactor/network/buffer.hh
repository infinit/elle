#ifndef INFINIT_REACTOR_NETWORK_BUFFER_HH
# define INFINIT_REACTOR_NETWORK_BUFFER_HH

# include <cassert>
# include <cstring>

# include <string>

# include <reactor/network/fwd.hh>

namespace reactor
{
  namespace network
  {
    class Buffer
    {
      public:
        Buffer(const Byte* data, Size size)
          : _data(const_cast<Byte*>(data))
          , _size(size)
        {}

        Buffer(const char* data, Size size)
          : _data(const_cast<Byte*>(reinterpret_cast<const Byte*>(data)))
          , _size(size)
        {}

        Buffer(const std::string& data)
          : _data(reinterpret_cast<Byte*>(const_cast<char*>(data.c_str())))
          , _size(data.size())
        {}

        Buffer(const Buffer& other)
          : _data(other._data)
          , _size(other._size)
        {}

        ~Buffer()
        {}

        Size size()
        {
          return _size;
        }

        Byte* data()
        {
          return _data;
        }

        Byte& operator[](Size pos)
        {
          assert(pos < _size);
          return _data[pos];
        }

      private:
        Byte* _data;
        Size _size;
    };
  }
}

#endif
