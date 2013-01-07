#ifndef ETOILE_ABSTRACT_GROUP_HH
# define ETOILE_ABSTRACT_GROUP_HH

# include <elle/types.hh>
# include <elle/utility/Time.hh>

# include <cryptography/PublicKey.hh>

# include <nucleus/proton/Revision.hh>
# include <nucleus/neutron/fwd.hh>

# include <elle/idiom/Open.hh>

// XXX[temporary: for cryptography]
using namespace infinit;

namespace etoile
{
  namespace abstract
  {

    ///
    /// this class describes an group's meta data.
    ///
    class Group:
      public elle::radix::Object
    {
    public:
      //
      // constructors & destructors
      //
      Group();
      ~Group();

      //
      // method
      //
      elle::Status      Create(const nucleus::neutron::Group&);

      //
      // interfaces
      //

      // group
      declare(Group);
      // XXX[useless: to remove]
      elle::Boolean     operator==(const Group&) const;

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //

      elle::String description;

      struct
      {
        elle::utility::Time creation;
        elle::utility::Time modification;
      } timestamps;

      nucleus::neutron::Size size;
      cryptography::PublicKey* manager;
      nucleus::proton::Revision revision;
      cryptography::PublicKey* pass_K;
    };

  }
}

# include <etoile/abstract/Group.hxx>

#endif
