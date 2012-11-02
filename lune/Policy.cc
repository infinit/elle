#include <horizon/Policy.hh>

namespace horizon
{

  std::ostream&
  operator <<(std::ostream& stream, Policy policy)
  {
    switch (policy)
      {
      case Policy::accessible:
        {
          stream << "accessible";
          break;
        }
      case Policy::editable:
        {
          stream << "editable";
          break;
        }
      case Policy::confidential:
        {
          stream << "confidential";
          break;
        }
      }

    return (stream);
  }

}
