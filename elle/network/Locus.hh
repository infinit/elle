#ifndef ELLE_NETWORK_LOCUS_HH
# define ELLE_NETWORK_LOCUS_HH

# include <elle/types.hh>

# include <elle/radix/Object.hh>

# include <elle/network/Host.hh>
# include <elle/network/Port.hh>

# include <elle/idiom/Open.hh>

namespace elle
{
  namespace network
  {

    ///
    /// this class represents a network locus i.e a host and port.
    ///
    class Locus:
      public radix::Object
    {
    public:
      //
      // constants
      //
      static const Locus                Null;

      //
      // constructors & destructors
      //
      Locus();

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
    };

  }
}

#include <elle/network/Locus.hxx>

#endif
