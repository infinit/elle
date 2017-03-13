#ifndef ELLE_PRINTABLE_HH
# define ELLE_PRINTABLE_HH

# include <iosfwd>

# include <elle/compiler.hh>

namespace elle
{
  /// Objects with a pretty string representation.
  ///
  /// Subclasses shall override print to define the pretty
  /// representation and will be printable through the << operator.
  class ELLE_API Printable
  {
  public:
    /// Print pretty representation to \a stream.
    ///
    /// @param The std::ostream.
    virtual
    void
    print(std::ostream& stream) const = 0;

    /// The pretty representation.
    std::string
    stringify() const;

    /// Non virtual version.
    template <typename T>
    struct as
    {};

  /*-------------.
  | Construction |
  `-------------*/
  protected:
    /// Destroy a Printable.
    ~Printable() = default;
  };

  /// Print pretty representation of \a o to \a stream.
  ELLE_API
  std::ostream&
  operator << (std::ostream& stream,
               Printable const& o);

  /// Print pretty representation of \a o to \a stream.
  template <typename T>
  ELLE_API
  std::ostream&
  operator << (std::ostream& stream,
               Printable::as<T> const& o)
  {
    static_cast<T const&>(o).print(stream);
    return stream;
  }
}

#endif
