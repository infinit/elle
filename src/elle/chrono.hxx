#include <elle/err.hh>
#include <elle/printf.hh>

namespace elle
{
  namespace chrono
  {
#define ELLE_CHRONO_DURATION_PARSE(Res)                                 \
      auto const pos = repr.find_first_not_of("-0123456789");           \
      if (pos == 0)                                                     \
        elle::err("invalid duration: %s", repr);                        \
      if (pos != std::string::npos)                                     \
      {                                                                 \
        auto count = std::atoi(repr.substr(0, pos).c_str());            \
        auto const unit = repr.substr(pos);                             \
        if (unit == "d")                                                \
          Res(count, 60 * 60 * 24, 1);                                  \
        else if (unit == "h")                                           \
          Res(count, 60 * 60, 1);                                       \
        else if (unit == "min")                                         \
          Res(count, 60, 1);                                            \
        else if (unit == "s")                                           \
          Res(count, 1, 1);                                             \
        else if (unit == "ms")                                          \
          Res(count, 1, 1000);                                          \
        else if (unit == "us")                                          \
          Res(count, 1, 1'000'000);                                     \
        else if (unit == "ns")                                          \
          Res(count, 1, 1000'000'000);                                  \
        else if (unit == "ps")                                          \
          Res(count, 1, 1'000'000'000'000);                             \
        else if (unit == "fs")                                          \
          Res(count, 1, 1000'000'000'000'000);                          \
        else                                                            \
          elle::err("invalid duration unit: %s", unit);                 \
      }                                                                 \
      else                                                              \
        Res(std::stoi(repr), 1, 1);

    template <typename Ratio, typename R>
    std::chrono::duration<R, Ratio>
    duration_parse(std::string const& repr)
    {
      using Res = std::chrono::duration<R, Ratio>;
#define ELLE_CHRONO_DURATION_PARSE_STATIC(Ticks, Num, Denom)            \
      return std::chrono::duration_cast<Res>(                           \
        std::chrono::duration<R, std::ratio<Num, Denom>>(Ticks))
      ELLE_CHRONO_DURATION_PARSE(ELLE_CHRONO_DURATION_PARSE_STATIC);
#undef ELLE_CHRONO_DURATION_PARSE_STATIC
    }
  }
}
