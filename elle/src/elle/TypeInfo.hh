#ifndef ELLE_TYPE_INFO_HH
# define ELLE_TYPE_INFO_HH

# include <string>
# include <typeindex>
# include <typeinfo>

# include <elle/attribute.hh>
# include <elle/compiler.hh>

namespace elle
{
  class ELLE_API TypeInfo
  {
  public:
    std::string
    name() const;
    bool
    operator ==(TypeInfo const& rhs) const;
    bool
    operator <(TypeInfo const& rhs) const;
    struct
    RegisterAbbrevation
    {
    public:
      RegisterAbbrevation(std::string const& full, std::string const& abbr);
    };

  private:
    TypeInfo(std::type_info const* info);
    template <typename T>
    friend TypeInfo type_info();
    template <typename T>
    friend TypeInfo type_info(T const&);
    ELLE_ATTRIBUTE(std::type_index, info);
    friend struct std::hash<TypeInfo>;
  };

  ELLE_API
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
  struct ELLE_API hash<elle::TypeInfo>
  {
  public:
    size_t operator()(elle::TypeInfo const& info) const;
  };
}

# include <elle/TypeInfo.hxx>

#endif
