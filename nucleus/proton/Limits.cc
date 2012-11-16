#include <nucleus/proton/Limits.hh>

#include <iostream>

namespace nucleus
{
  namespace proton
  {

//
// ---------- constructors & destructor ---------------------------------------
//

    Limits::Limits():
      _extent(0),
      _contention(0.0),
      _balancing(0.0)
    {
    }

    Limits::Limits(elle::Natural32 const extent,
                   elle::Real const contention,
                   elle::Real const balancing):
      _extent(extent),
      _contention(contention),
      _balancing(balancing)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    elle::Natural32
    Limits::extent() const
    {
      return (this->_extent);
    }

    elle::Real
    Limits::contention() const
    {
      return (this->_contention);
    }

    elle::Real
    Limits::balancing() const
    {
      return (this->_balancing);
    }

//
// ---------- dumpable --------------------------------------------------------
//

    elle::Status Limits::Dump(const elle::Natural32 margin) const
    {
      elle::String alignment(margin, ' ');

      std::cout << alignment << "[Limits]" << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Extent] " << std::dec << this->_extent
                << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Contention] " << std::dec << this->_contention
                << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Balancing] " << std::dec << this->_balancing
                << std::endl;

      return elle::Status::Ok;
    }

  }
}
