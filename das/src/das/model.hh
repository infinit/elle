#ifndef DAS_MODEL_CONVENIENCE_HH
# define DAS_MODEL_CONVENIENCE_HH

namespace das
{
  template <typename T>
  struct Das
  {};
}

# define DAS_MODEL(T, M)                        \
  namespace das                                 \
  {                                             \
    template <>                                 \
      struct Das<T>                             \
    {                                           \
      typedef M Model;                          \
    };                                          \
  }

# include <das/Object.hh>
# include <das/Collection.hh>
# include <das/Variable.hh>
# include <das/printer.hh>

#endif
