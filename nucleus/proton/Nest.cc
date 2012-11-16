#include <nucleus/proton/Nest.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- constructors & destructors --------------------------------------
//

    Nest::Nest(Limits const& limits):
      _limits(limits)
    {
    }

    Nest::~Nest()
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    Limits const&
    Nest::limits() const
    {
      return (this->_limits);
    }

  }
}
