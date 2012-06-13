#include <elle/Buffer.hh>

namespace elle
{
  Buffer::Buffer(Data data, Size size)
    : size(size)
    , data(data)
  {}
}
