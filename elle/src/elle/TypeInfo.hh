#ifndef ELLE_TYPE_INFO_HH
# define ELLE_TYPE_INFO_HH

# include <string>
# include <typeindex>
# include <typeinfo>

# include <elle/attribute.hh>

namespace elle
{
  class TypeInfo
  {
  public:
    std::string
    name() const;
    bool
    operator ==(TypeInfo const& rhs) const;
    bool
    operator <(TypeInfo const& rhs) const;

  private:
    TypeInfo(std::type_info const* info);
    template <typename T>
    friend TypeInfo type_info();
    template <typename T>
    friend TypeInfo type_info(T const&);
    ELLE_ATTRIBUTE(std::type_index, info);
# ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wmismatched-tags"
# endif
    friend class std::hash<TypeInfo>;
# ifdef __clang__
#  pragma clang diagnostic pop
# endif
  };

  std::ostream&
  operator << (std::ostream& s, TypeInfo const& ti);

  template <typename T>
  TypeInfo
  type_info();

  template <typename T>
  TypeInfo
  type_info(T const& v);
}

namespace std
{
  template<>
  class hash<elle::TypeInfo>
  {
  public:
    size_t operator()(elle::TypeInfo const& info) const;
  };
}

# include <elle/TypeInfo.hxx>

#endif
