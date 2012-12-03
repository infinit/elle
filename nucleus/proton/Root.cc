#include <nucleus/proton/Root.hh>

namespace nucleus
{
  namespace proton
  {
    /*-------------.
    | Construction |
    `-------------*/

    Root::Root()
    {
    }

    Root::Root(Address const& address,
               Height const& height,
               Capacity const& capacity):
      _address(address),
      _height(height),
      _capacity(capacity)
    {
    }
  }
}
