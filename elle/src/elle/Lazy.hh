#ifndef ELLE_LAZY_HH
# define ELLE_LAZY_HH

# include <boost/optional.hpp>

# include <elle/attribute.hh>

namespace elle
{
  /// A wrapper that evaluates its value the first time it's needed, if ever.
  ///
  ///
  template <typename T>
  class Lazy
  {
  /*------.
  | Types |
  `------*/
  public:
    /// Encapsulated value type.
    typedef T Value;
    /// Self type.
    typedef Lazy<Value> Self;
    /// Function type to generate our value.
    typedef std::function<Value ()> Lambda;

  /*-------------.
  | Construction |
  `-------------*/
  public:
    /// Build a lazy that evaluates to the return value of \a lambda.
    Lazy(Lambda const& lambda);

  /*------.
  | Value |
  `------*/
  public:
    /// The mutable value.
    Value&
    value();
    /// The constant value.
    Value const&
    value() const;
    /// Apply a mutable operation to the value.
    Value*
    operator -> ();
    /// Apply a constant operation to the value.
    Value const*
    operator -> () const;

  private:
    /// The lambda that yields our value.
    ELLE_ATTRIBUTE(Lambda, lambda);
    /// The value, if evaluated.
    ELLE_ATTRIBUTE(boost::optional<Value>, value);
  };

  /// Print the value of \a lazy into \a output.
  ///
  /// \param  output The stream to print to.
  /// \param  lazy   The Lazy whose value to print.
  /// \return \a output
  template <typename T>
  std::ostream&
  operator << (std::ostream& output, Lazy<T> const& lazy);

  /// Helper to create lazy values.
  ///
  /// \return Lazy(\a f).
  template <typename F>
  auto
  lazy(F&& f) -> Lazy<decltype(f())>;

}

# include <elle/Lazy.hxx>

#endif
