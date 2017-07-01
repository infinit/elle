#include <elle/chrono.hh>

namespace elle
{
  namespace chrono
  {
    void
    duration_parse(std::string const& repr,
                   std::int64_t& ticks,
                   std::int64_t& num,
                   std::int64_t& denom)
    {
#define ELLE_CHRONO_DURATION_PARSE_DYNAMIC(Ticks, Num, Denom)   \
      do {                                                      \
        ticks = Ticks;                                          \
        num = Num;                                              \
        denom = Denom;                                          \
      } while (false)

      ELLE_CHRONO_DURATION_PARSE(ELLE_CHRONO_DURATION_PARSE_DYNAMIC);
#undef ELLE_CHRONO_DURATION_PARSE_DYNAMIC
    }
  }
}
