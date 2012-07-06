#ifndef NUCLEUS_NEUTRON_ENTRY_HH
# define NUCLEUS_NEUTRON_ENTRY_HH

# include <elle/types.hh>
# include <elle/radix/Object.hh>

# include <nucleus/proton/Address.hh>

# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace neutron
  {

    ///
    /// this class represents a directory i.e catalog entry which is
    /// composed of a name and its object's corresponding address.
    ///
    class Entry:
      public elle::radix::Object
    {
    public:
      //
      // constants
      //
      static const Entry                Null;

      //
      // types
      //
      typedef elle::String              S;

      //
      // constructors & destructors
      //
      Entry();
      Entry(const elle::String&,
            const proton::Address&);

      //
      // interfaces
      //

      // object
      declare(Entry);
      elle::Boolean     operator==(const Entry&) const;

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // rangeable
      elle::String&     Symbol();

      //
      // attributes
      //
      elle::String      name;
      proton::Address   address;
    };

  }
}

#include <nucleus/neutron/Entry.hxx>

#endif
