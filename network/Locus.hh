#ifndef ELLE_NETWORK_LOCUS_HH
# define ELLE_NETWORK_LOCUS_HH

# include <elle/types.hh>
# include <elle/operator.hh>
# include <elle/Printable.hh>

# include <elle/network/Host.hh>
# include <elle/network/Port.hh>

# include <utility>
ELLE_OPERATOR_RELATIONALS();

namespace elle
{
  namespace network
  {

    /// A network host and port.
    class Locus:
      public elle::Printable
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
      Locus(Locus const&) = default;

      //
      // methods
      //
      Status            Create(const String&);
      Status            Create(const Host&,
                               const Port);

      //
      // interfaces
      //

      ELLE_OPERATOR_ASSIGNMENT(Locus); // XXX

      Boolean           operator==(const Locus&) const;
      Boolean           operator<(const Locus&) const;

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
