#ifndef ETOILE_ABSTRACT_GROUP_HH
# define ETOILE_ABSTRACT_GROUP_HH

# include <elle/types.hh>
# include <elle/utility/Time.hh>
# include <elle/cryptography/PublicKey.hh>

# include <nucleus/proton/Version.hh>
# include <nucleus/neutron/fwd.hh>

# include <elle/idiom/Open.hh>

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
      } stamps;

      nucleus::neutron::Size size;
      elle::cryptography::PublicKey manager;
      nucleus::proton::Version version;
      elle::cryptography::PublicKey pass_K;
    };

  }
}

# include <etoile/abstract/Group.hxx>

#endif
