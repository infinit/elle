#pragma once

#include <string>

#include <elle/Buffer.hh>
#include <elle/assert.hh>
#include <elle/reactor/network/fwd.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /// A non-owning zone of memory.
      ///
      /// Typically used with an automatic array for network buffers.
      /// Of course, the given memory must survive to these buffers.
      class Buffer
      {
      public:
        Buffer(Byte* data, Size size)
          : _data(data)
          , _size(size)
        {}

        Buffer(const Byte* data, Size size)
          : Buffer(const_cast<Byte*>(data), size)
        {}

        Buffer(const char* data, Size size)
          : Buffer(reinterpret_cast<const Byte*>(data), size)
        {}

        Buffer(const std::string& data)
          : Buffer(data.data(), data.size())
        {}

        Buffer(const Buffer& other)
          : Buffer(other._data, other._size)
        {}

        Buffer(const elle::Buffer& data)
          : Buffer(data.mutable_contents(), data.size())
        {}

        ~Buffer() = default;

        Size size() const
        {
          return _size;
        }

        Byte* data()
        {
          return _data;
        }

        Byte& operator[](Size pos)
        {
          ELLE_ASSERT_LT(pos, _size);
          return _data[pos];
        }

      private:
        Byte* _data;
        Size _size;
      };
    }
  }
}
