#include <reactor/duration.hh>

namespace reactor
{
  std::ostream&
  operator << (std::ostream& s, const DurationOpt& opt)
  {
    if (opt)
      s << opt.get();
    else
      s << "no duration";
  }
}
