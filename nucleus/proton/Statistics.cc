#include <nucleus/proton/Statistics.hh>

#include <limits>

namespace nucleus
{
  namespace proton
  {

    /*-------------.
    | Construction |
    `-------------*/

    Statistics::Statistics():
      _footprint_minimum(std::numeric_limits<Footprint>::max()),
      _footprint_average(0),
      _footprint_maximum(std::numeric_limits<Footprint>::min()),
      _capacity_minimum(std::numeric_limits<Capacity>::max()),
      _capacity_average(0),
      _capacity_maximum(std::numeric_limits<Capacity>::min()),
      _blocks_clean(0),
      _blocks_dirty(0),
      _blocks_consistent(0),
      _blocks_all(0)
    {
    }

    /*---------.
    | Dumpable |
    `---------*/

    elle::Status
    Statistics::Dump(const elle::Natural32 margin) const
    {
      elle::String alignment(margin, ' ');

      std::cout << alignment << "[Statistics]" << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Footprint]" << std::endl;
      {
        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Minimum] " << std::dec
                  << this->_footprint_minimum << std::endl;

        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Average] " << std::dec
                  << this->_footprint_average << std::endl;

        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Maximum] " << std::dec
                  << this->_footprint_maximum << std::endl;
      }

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Capacity]" << std::endl;
      {
        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Minimum] " << std::dec
                  << this->_capacity_minimum << std::endl;

        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Average] " << std::dec
                  << this->_capacity_average << std::endl;

        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Maximum] " << std::dec
                  << this->_capacity_maximum << std::endl;
      }

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Blocks]" << std::endl;
      {
        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Clean] " << std::dec
                  << this->_blocks_clean << std::endl;

        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Dirty] " << std::dec
                  << this->_blocks_dirty << std::endl;

        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Consistent] " << std::dec
                  << this->_blocks_consistent << std::endl;

        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[All] " << std::dec
                  << this->_blocks_all << std::endl;
      }

      return (elle::Status::Ok);
    }
  }
}
