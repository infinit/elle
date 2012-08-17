#include <hole/Openness.hh>

# include <iostream>

namespace hole
{

  std::ostream&
  operator <<(std::ostream& stream, Openness openness)
  {
    switch (openness)
      {
      case Openness::open:
        {
          stream << "open";
          break;
        }
      case Openness::community:
        {
          stream << "community";
          break;
        }
      case Openness::closed:
        {
          stream << "closed";
          break;
        }
      }

    return (stream);
  }

}
