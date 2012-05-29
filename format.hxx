#ifndef  ELLE_FORMAT_HXX
# define ELLE_FORMAT_HXX

# include <boost/format.hpp>

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
  }

  // XXX virer les const& et utiliser les bon traits
  template<typename... T>
  std::string
    format(char const* fmt, T const&... values)
    {
      boost::format boost_fmt{fmt};
      detail::format_feed(boost_fmt, values...);
      return boost_fmt.str();
    }

}

#endif

