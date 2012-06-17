#include <reactor/duration.hh>

namespace reactor
{
  std::ostream&
  operator << (std::ostream& s, const DurationOpt& opt)
  {
    if (opt)
      return s << opt.get();
    else
      return s << "no duration";
  }
}
