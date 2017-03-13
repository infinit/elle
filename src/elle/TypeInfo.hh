#ifndef ELLE_TYPE_INFO_HH
# define ELLE_TYPE_INFO_HH

# include <string>
# include <typeindex>
# include <typeinfo>

# include <elle/attribute.hh>
# include <elle/compiler.hh>

namespace elle
{
  /// An extended std::type_info like.
  ///
  /// \code{.cc}
  ///
  /// int i = 3;
  /// std::cout << elle::type_info(i) << "\n";
  ///
  /// struct Foo
  /// {
  ///   struct Bar
  ///   {
  ///   };
  /// };
  /// std::cout << elle::type_info<Foo::Bar>();
  ///
  /// //Result: int
  ///           Foo::Bar
  ///
  /// \endcode
  class ELLE_API TypeInfo
  {
  public:
    /// Return the demangled name (if possible) of the given type.
    std::string
    name() const;
    /// Check if two TypeInfos are equal.
    bool
    operator ==(TypeInfo const& rhs) const;
    /// Check order of two TypeInfos according to their hash.
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

  /// Print a TypeInfo. Use the abbreviation if any.
  ELLE_API
  std::ostream&
  operator << (std::ostream& s, TypeInfo const& ti);

  /// Return the TypeInfo for the given type.
  ///
  /// @tparam T The type.
  template <typename T>
  TypeInfo
  type_info();

  /// Return the TypeInfo for a given object.
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

# include <elle/TypeInfo.hxx>

#endif
