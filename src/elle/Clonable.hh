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
  protected:
    typedef Cloned _elle_Cloned;
  };
}

# define ELLE_CLONABLE()                                                \
  virtual                                                               \
  std::unique_ptr<typename Self::_elle_Cloned>                          \
  clone() const override                                                \
  {                                                                     \
    return                                                              \
      std::unique_ptr<typename Self::_elle_Cloned>(new Self(*this));    \
  }                                                                     \

#endif
