#ifndef ELLE_PRINTABLE_HH
# define ELLE_PRINTABLE_HH

# include <ostream>

namespace elle
{
  class Printable
  {
  public:
    virtual
    ~Printable();

    virtual
    void
    print(std::ostream& stream) const = 0;

    std::string
    stringify() const;
  };

  std::ostream& operator << (std::ostream& stream,
                             Printable const& o);
}

#endif
