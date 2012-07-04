#ifndef ETOILE_MISCELLANEOUS_ABSTRACT_HH
# define ETOILE_MISCELLANEOUS_ABSTRACT_HH

# include <elle/types.hh>
# include <elle/utility/Time.hh>
# include <elle/cryptography/PublicKey.hh>

# include <nucleus/proton/Version.hh>
# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Genre.hh>
# include <nucleus/neutron/Offset.hh>
# include <nucleus/neutron/Permissions.hh>

# include <elle/idiom/Open.hh>

namespace etoile
{
  namespace miscellaneous
  {

    ///
    /// this class describes an object's meta data.
    ///
    class Abstract:
      public elle::radix::Object
    {
    public:
      //
      // constants
      //
      static const Abstract             Null;

      //
      // constructors & destructors
      //
      Abstract();

      //
      // method
      //
      elle::Status      Create(const nucleus::neutron::Object&);

      //
      // interfaces
      //

      // object
      declare(Abstract);
      elle::Boolean     operator==(const Abstract&) const;

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      nucleus::neutron::Genre genre;

      struct
      {
        elle::utility::Time creation;
        elle::utility::Time modification;
      } stamps;

      nucleus::neutron::Offset size;

      struct
      {
        elle::cryptography::PublicKey owner;
        elle::cryptography::PublicKey author;
      } keys;

      struct
      {
        nucleus::neutron::Permissions owner;
      } permissions;

      struct
      {
        nucleus::proton::Version data;
        nucleus::proton::Version meta;
      } versions;
    };

  }
}

# include <etoile/miscellaneous/Abstract.hxx>

#endif
