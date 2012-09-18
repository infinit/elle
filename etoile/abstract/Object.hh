#ifndef ETOILE_ABSTRACT_OBJECT_HH
# define ETOILE_ABSTRACT_OBJECT_HH

# include <elle/types.hh>
# include <elle/utility/Time.hh>
# include <elle/cryptography/PublicKey.hh>

# include <nucleus/proton/Revision.hh>
# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Genre.hh>
# include <nucleus/neutron/Size.hh>
# include <nucleus/neutron/Permissions.hh>

# include <elle/idiom/Open.hh>

namespace etoile
{
  namespace abstract
  {

    ///
    /// this class describes an object's meta data.
    ///
    class Object:
      public elle::radix::Object
    {
    public:
      //
      // constructors & destructors
      //
      Object();

      //
      // method
      //
      elle::Status      Create(const nucleus::neutron::Object&);

      //
      // interfaces
      //

      // object
      declare(Object);
      // XXX[useless: to remove]
      elle::Boolean     operator==(const Object&) const;

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

      nucleus::neutron::Size size;

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
        nucleus::proton::Revision data;
        nucleus::proton::Revision meta;
      } revisions;
    };

  }
}

# include <etoile/abstract/Object.hxx>

#endif
