#ifndef ELLE_CLONABLE_HH
# define ELLE_CLONABLE_HH

# include <memory>

namespace elle
{
  template <typename Cloned>
  class Clonable
  {
    public:
      virtual
      std::unique_ptr<Cloned>
      clone() const = 0;
  };
}

#endif
