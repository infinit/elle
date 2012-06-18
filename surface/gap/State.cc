#include <stdexcept>

#include "State.hh"

namespace surface
{
  namespace gap
  {

    struct Network
    {
      std::string name;
    };

    State::~State()
    {
      for (auto& it: this->_networks)
        delete (it).second;
      this->_networks.clear();
    }

    void State::refresh_networks()
    {
    }

    std::string const& State::path_to_network(std::string const& path) const
    {
      throw std::runtime_error("Cannot find any network for path '" + path + "'");
    };

  }
}

