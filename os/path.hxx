#ifndef  ELLE_OS_PATH_HXX
# define ELLE_OS_PATH_HXX

# include <vector>

namespace elle
{
  namespace os
  {
    namespace path
    {

      namespace detail
      {
        std::string join(std::vector<std::string> const& strings);
      }

      template<typename... T>
      std::string join(T const&... parts)
      {
        static_assert(sizeof...(parts) > 0, "join() needs at least one part.");
        std::vector<std::string> strings{parts...};
        return detail::join(strings);
      }

    }
  }
}

#endif
