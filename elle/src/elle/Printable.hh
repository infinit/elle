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
    virtual
    void
    print(std::ostream& stream) const = 0;

    /// The pretty representation.
    std::string
    stringify() const;

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
}

#endif
