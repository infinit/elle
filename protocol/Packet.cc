#include <sstream>

#include <protocol/Packet.hh>

namespace infinit
{
  namespace protocol
  {
    /*-------------.
    | Construction |
    `-------------*/

    Packet::Packet(Byte* data, Size size)
      : _data(data)
      , _data_size(size)
    {}

    Packet::Packet(Packet&& source)
      : _data(source._data)
      , _data_size(source._data_size)
    {
      source._data = 0;
      source._data_size = 0;
    }

    Packet::~Packet()
    {
      delete [] _data;
    }

    /*-----------.
    | Interfaces |
    `-----------*/

    std::unique_ptr<std::istream>
    Packet::stream() const
    {
      // FIXME: A custom stream would be *way* faster
      std::string content(reinterpret_cast<char*>(_data), _data_size);
      auto res = new std::stringstream(content);
      return std::unique_ptr<std::istream>(res);
    }

    /*--------.
    | Details |
    `--------*/

    Packet::Packet(Size data_size)
      : _data(new Byte[data_size])
      , _data_size(data_size)
    {}
  }
}
