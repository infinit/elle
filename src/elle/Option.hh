#pragma once

#include <cstddef>
#include <type_traits>
#include <iosfwd>

#include <elle/serialization/Serializer.hh>

namespace elle
{
  namespace _details
  {
    template <std::size_t Size,
              std::size_t Align,
              int Index,
              typename ... Types>
    class OptionHelper;
  }

  /// A generic type, based on a finite subset of types.
  ///
  /// XXX: Is this relevant regarding the existence of boost::variant and the
  ///      C++17 std::variant?
  ///
  /// \code{.cc}
  ///
  /// using MyOption = typename elle::Option<int, bool, std::string>;
  ///
  /// void
  /// inspect(MyOption const& e)
  /// {
  ///   std::cout << "int: " << e.is<int>() << '\n'
  ///             << "string: " << e.is<std::string>() << '\n'
  ///             << "bool: " << e.is<bool>() << '\n';
  /// }
  ///
  /// MyOption option1(42);
  /// MyOption option2(std::string("forty-two"));
  /// inspect(option1);
  /// std::cout << '\n';
  /// inspect(option2);
  ///
  /// // Result.
  /// int: 1
  /// string: 0
  /// bool: 0
  ///
  /// int: 0
  /// string: 1
  /// bool: 0
  ///
  /// \endcode
  template <typename ... Types>
  class Option
    : public _details::OptionHelper<0u, 0u, 0, Types ...>
  {
  public:
    using Self = Option<Types ...>;
    using Super =  _details::OptionHelper<0u, 0u, 0, Types ...>;
    using Super::Super;
    /// Create an Option.
    ///
    /// @tparam T A type.
    /// @param value An r-value reference.
    template <typename T>
    Option(T&& value);
    /// Copy-constructor.
    Option(Self const& source);
    /// Copy-constructor.
    Option(Self& source);
    /// Move-constructor.
    Option(Self&& source);
    /// Assignment operator.
    Self&
    operator =(Self&& option);
    /// Assignment operator.
    Self&
    operator =(Self const& option);
    /// Try to get the underlying value as a given Type.
    ///
    /// @tparam T The expected type.
    /// @return A reference to the underlying value.
    template <typename T>
    T&
    get();
    /// Try to get the underlying value as a given Type.
    ///
    /// @tparam T The expected type.
    /// @return A const-reference to the underlying value.
    template <typename T>
    T const&
    get() const;
    /// Check if the type of the value of the Option is the one given.
    ///
    /// @tparam T The expected type.
    /// @return Whether the underlying value is from the given type.
    template <typename T>
    bool
    is() const;
    /// Update the underlying value.
    template <typename T>
    void
    emplace(T&& value);
    /// Update the value.
    template <typename T, typename ... Args>
    void
    emplace(Args&& ... value);

  /*--------------.
  | Serialization |
  `--------------*/
  public:
    using serialization_tag = elle::serialization_tag;
    Option(serialization::SerializerIn& s);
    void
    serialize(serialization::Serializer& s);

  /*----------.
  | Printable |
  `----------*/
  private:
    template <typename ... Args>
    friend
    std::ostream&
    operator << (std::ostream& output, Option<Args...> const& option);
  };
}

#include <elle/Option.hxx>
