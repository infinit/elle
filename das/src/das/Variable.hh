#ifndef DAS_VARIABLE_HH
# define DAS_VARIABLE_HH

# include <boost/signals2.hpp>

namespace das
{
  template <typename T>
  class Variable
  {
  public:
    Variable();
    Variable(T const& value);
    Variable(T&& value);
    template <typename U>
    Variable(Variable<U> const& value);
    Variable(Variable<T> const& value);
    Variable<T>&
    operator =(T const& value);
    template <typename U>
    Variable<T>&
    operator =(Variable<U> const& value);
    Variable<T>&
    operator =(Variable<T> const& value);
    template <typename U>
    bool
    operator ==(Variable<U> const& other) const;
    operator T const& () const;
    ELLE_ATTRIBUTE_R(T, value);
    ELLE_ATTRIBUTE_RX(boost::signals2::signal<void (T const&)>, changed);
  };

  template <typename T>
  std::ostream&
  operator << (std::ostream& o, Variable<T> const& v);
}

# include <das/Variable.hxx>

#endif
