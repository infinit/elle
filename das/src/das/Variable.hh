#ifndef DAS_VARIABLE_HH
# define DAS_VARIABLE_HH

#include <elle/serialization/Serializer.hh>

namespace das
{
  template <typename T>
  class Variable
  {
  public:
    Variable<T>&
    operator =(T const& value);
    ELLE_ATTRIBUTE_R(T, value);
  };
}

# include <das/Variable.hxx>

#endif
