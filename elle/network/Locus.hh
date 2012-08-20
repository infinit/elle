#ifndef ELLE_NETWORK_LOCUS_HH
# define ELLE_NETWORK_LOCUS_HH

# include <elle/Printable.hh>
# include <elle/types.hh>

# include <elle/radix/Object.hh>

# include <elle/network/Host.hh>
# include <elle/network/Port.hh>

# include <elle/idiom/Open.hh>

namespace elle
{
  namespace network
  {

    /// A network host and port.
    class Locus
      : public radix::Object
      , public elle::Printable
    {
    public:
      //
      // constants
      //
      static const Locus                Null;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      Locus();
      Locus(std::string const& hostname, int port);

      //
      // methods
      //
      Status            Create(const String&);
      Status            Create(const Host&,
                               const Port);

      //
      // interfaces
      //

      // object
      declare(Locus);
      Boolean           operator==(const Locus&) const;
      Boolean           operator<(const Locus&) const;
      Boolean           operator>(const Locus&) const;

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Host              host;
      Port              port;

    /*----------.
    | Printable |
    `----------*/
    public:
      virtual void print(std::ostream& stream) const;
    };

  }
}



#include <elle/network/Locus.hxx>

#endif
