#ifndef DAS_VARIABLE_HH
# define DAS_VARIABLE_HH

namespace das
{
  template <typename T>
  class Variable
  {
  public:
    Variable();
    Variable(T const& value);
    Variable(T&& value);
    Variable<T>&
    operator =(T const& value);
    ELLE_ATTRIBUTE_R(T, value);
  };

  template <typename T>
  std::ostream&
  operator << (std::ostream& o, Variable<T> const& v);
}

# include <das/Variable.hxx>

#endif
