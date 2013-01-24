#ifndef  ELLE_MEMORY_HH
# define ELLE_MEMORY_HH

# include <memory>

namespace elle
{
  template<typename T, typename... Args>
  std::unique_ptr<T>
  make_unique(Args&&... args)
  {
    return std::unique_ptr<T>{new T{std::forward<Args>(args)...}};
  }

}

#endif

