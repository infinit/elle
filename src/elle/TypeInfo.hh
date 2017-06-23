#pragma once

#include <string>
#include <typeindex>
#include <typeinfo>

#include <boost/operators.hpp>

#include <elle/attribute.hh>
#include <elle/compiler.hh>

namespace elle
{
  /// An extended std::type_info like.
  ///
  /// @code{.cc}
  ///
  /// int i = 3;
  /// std::cout << elle::type_info(i) << '\n';
  ///
  /// struct Foo
  /// {
  ///   struct Bar {};
  /// };
  /// std::cout << elle::type_info<Foo::Bar>() << '\n';
  ///
  /// //Result: int
  ///           Foo::Bar
  ///
  /// @endcode
  class ELLE_API TypeInfo
    : private boost::totally_ordered<TypeInfo>
  {
  public:
    /// The demangled name (if possible) of the given type.
    std::string
    name() const;
    /// Whether two TypeInfos are equal.
    bool
    operator ==(TypeInfo const& rhs) const;
    /// Check order of two TypeInfos.
    bool
    operator <(TypeInfo const& rhs) const;
    /// Register an abbreviation for a given type.
    struct
    RegisterAbbrevation
    {
    public:
      RegisterAbbrevation(std::string const& full, std::string const& abbr);
    };

  private:
    /// Construct a TypeInfo.
    ///
    /// This constructor is private, use elle::type_info.
    ///
    /// @see elle::type_info().
    TypeInfo(std::type_info const* info);
    template <typename T>
    friend TypeInfo type_info();
    template <typename T>
    friend TypeInfo type_info(T const&);
    ELLE_ATTRIBUTE(std::type_index, info);
    friend struct std::hash<TypeInfo>;
  };

  /// Print a TypeInfo. Use abbreviations in fixed format (%f, not %s).
  ELLE_API
  std::ostream&
  operator << (std::ostream& s, TypeInfo const& ti);

  /// Return the TypeInfo for the given type.
  ///
  /// @tparam T The type.
  template <typename T>
  TypeInfo
  type_info();

  /// The TypeInfo for a given object.
  ///
  /// @tparam T The type.
  /// @param v A variable.
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

/// Declare that `Long` should be displayed as `Short`.
#define ELLE_TYPE_INFO_ABBR(Short, Long)                                \
  namespace                                                             \
  {                                                                     \
    const auto BOOST_PP_CAT(_elle_typeinfo_abbr, __LINE__) =            \
      elle::TypeInfo::RegisterAbbrevation(Long, Short);                 \
  }

#include <elle/TypeInfo.hxx>
