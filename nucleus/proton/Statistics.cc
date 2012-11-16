#include <nucleus/proton/Statistics.hh>

#include <elle/idiom/Close.hh>
# include <limits>
#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- construction ----------------------------------------------------
//

    Statistics::Statistics()
    {
      this->footprint.minimum = std::numeric_limits<Footprint>::max();
      this->footprint.average = 0;
      this->footprint.maximum = std::numeric_limits<Footprint>::min();

      this->capacity.minimum = std::numeric_limits<Capacity>::max();
      this->capacity.average = 0;
      this->capacity.maximum = std::numeric_limits<Capacity>::min();

      this->blocks.clean = 0;
      this->blocks.dirty = 0;
      this->blocks.consistent = 0;
      this->blocks.all = 0;
    }

//
// ---------- dumpable --------------------------------------------------------
//

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
                  << this->footprint.minimum << std::endl;

        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Average] " << std::dec
                  << this->footprint.average << std::endl;

        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Maximum] " << std::dec
                  << this->footprint.maximum << std::endl;
      }

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Capacity]" << std::endl;
      {
        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Minimum] " << std::dec
                  << this->capacity.minimum << std::endl;

        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Average] " << std::dec
                  << this->capacity.average << std::endl;

        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Maximum] " << std::dec
                  << this->capacity.maximum << std::endl;
      }

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Blocks]" << std::endl;
      {
        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Clean] " << std::dec
                  << this->blocks.clean << std::endl;

        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Dirty] " << std::dec
                  << this->blocks.dirty << std::endl;

        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Consistent] " << std::dec
                  << this->blocks.consistent << std::endl;

        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[All] " << std::dec
                  << this->blocks.all << std::endl;
      }

      return (elle::Status::Ok);
    }

  }
}
