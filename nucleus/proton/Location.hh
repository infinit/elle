#ifndef NUCLEUS_PROTON_LOCATION_HH
# define NUCLEUS_PROTON_LOCATION_HH

# include <elle/types.hh>
# include <elle/radix/Object.hh>

# include <nucleus/proton/Address.hh>
# include <nucleus/proton/Version.hh>

namespace nucleus
{
  namespace proton
  {

    ///
    /// this class represents a storage location both in space and time.
    ///
    /// indeed, a location is composed of the address of the object but
    /// also the number of the object's version which it relates to.
    ///
    class Location:
      public elle::radix::Object
    {
    public:
      //
      // constants
      //
      static const Location             Null;

      //
      // constructors & destructors
      //
      Location();

      //
      // methods
      //
      elle::Status      Create(const Address&,
                               const Version&);

      //
      // interfaces
      //

      // object
      declare(Location);
      elle::Boolean     operator==(const Location&) const;

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Address           address;
      Version           version;
    };

  }
}

#include <nucleus/proton/Location.hxx>

#endif
