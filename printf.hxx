#ifndef  ELLE_PRINTF_HXX
# define ELLE_PRINTF_HXX

# include <iostream>

# include <boost/format.hpp>

# include <elle/printf.hh>

namespace elle
{
  namespace detail
  {
    inline void
    format_feed(boost::format&)
    {}

    template<typename T, typename... K>
    void
    format_feed(boost::format& fmt, T const& front, K const&... tail)
    {
      fmt % front;
      format_feed(fmt, tail...);
    }

    // XXX virer les const& et utiliser les bon traits
    template<typename... T>
      std::string
      format(char const* fmt, T const&... values)
        {
          boost::format boost_fmt{fmt};
          format_feed(boost_fmt, values...);
          return boost_fmt.str();
        }
  }


  template<typename... T>
    size_t fprintf(std::ostream& out, char const* fmt, T const&... values)
    {
      std::string res = elle::detail::format(fmt, values...);
      out << res;
      return res.size();
    }

  template<typename... T>
    size_t printf(char const* fmt, T const&... values)
    {
      return fprintf(std::cout, fmt, values...);
    }

  template<typename... T>
    std::string sprintf(char const* fmt, T const&... values)
    {
      return elle::detail::format(fmt, values...);
    }

}

#endif
