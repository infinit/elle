#include <string>
#include <limits>
#include <stdexcept>

// You would think that an inline definition in <string> would
// suffice, but then you have nasty header include issues: it would
// have to include <stdexcept>, and on some platforms <stdexcept>
// includes <string>...  Very messy.  Make it simple.
namespace std
{
  // http://stackoverflow.com/q/8715213/1353549.
  unsigned
  stou(std::string const& str, size_t* idx, int base)
  {
    auto const res = std::stoul(str, idx, base);
    if (std::numeric_limits<unsigned>::max() < res)
      throw std::out_of_range("stou");
    else
      return res;
  }
}
