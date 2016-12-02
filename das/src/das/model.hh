#ifndef DAS_MODEL_CONVENIENCE_HH
# define DAS_MODEL_CONVENIENCE_HH

# include <elle/attribute.hh>
# include <elle/meta.hh>

# include <das/fwd.hh>

namespace das
{
  template <typename T, typename Fields_>
  class Model
  {
  private:
    template <typename A>
    struct AttrType
    {
      using type = typename A::template attr_type<T>::type;
    };

  public:
    using Fields = Fields_;
    using Types = typename Fields::template map<AttrType>::type;
  };

  namespace _details
  {
    template <typename T>
    static constexpr
    int
    _default_model(...)
    {
      return 0;
    }

    template <typename T>
    static constexpr
    std::enable_if_exists_t<typename T::Model::Fields, int>
    _default_model(int)
    {
      return 1;
    }

    template <int kind, typename T>
    struct DefaultModelHelper
    {};

    template <typename T>
    struct DefaultModelHelper<1, T>
    {
      using type = typename T::Model;
    };
  }

  template <typename T>
  constexpr inline
  bool
  model_has()
  {
    return _details::_default_model<T>(0);
  }

  template <typename T>
  struct DefaultModel
    : public _details::DefaultModelHelper<_details::_default_model<T>(0), T>
  {};
}

#define DAS_MODEL_DEFAULT(Class, Model)         \
namespace das                                   \
{                                               \
  template <>                                   \
  struct DefaultModel<Class>                    \
  {                                             \
  public:                                       \
    using type =                                \
      ELLE_ATTRIBUTE_STRIP_PARENS(Model);       \
  };                                            \
}                                               \

#endif
