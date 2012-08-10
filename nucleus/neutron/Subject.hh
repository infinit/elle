#ifndef NUCLEUS_NEUTRON_SUBJECT_HH
# define NUCLEUS_NEUTRON_SUBJECT_HH

# include <elle/types.hh>
# include <elle/radix/Object.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/neutron/User.hh>
# include <nucleus/neutron/Group.hh>

namespace nucleus
{
  namespace neutron
  {

    ///
    /// this class is used to represents a subject i.e an entity which
    /// can be granted access such as a user or a group.
    ///
    class Subject:
      public elle::radix::Object
    {
    public:
      //
      // enumeration
      //
      enum Type
        {
          TypeUnknown,

          TypeUser,
          TypeGroup,

          Types
        };

      //
      // structures
      //
      struct Descriptor
      {
        Type type;
        elle::String name;
      };

      //
      // static methods
      //
      static elle::Status       Convert(const elle::String&,
                                        Type&);
      static elle::Status       Convert(const Type,
                                        elle::String&);

      //
      // static attributes
      //
      static const Descriptor           Descriptors[Types];

      //
      // constructors & destructors
      //
      Subject();
      Subject(typename User::Identity const& identity);
      Subject(typename Group::Identity const& identity);
      Subject(Subject const& other);
      ~Subject();

      //
      // methods
      //
      elle::Status      Create(typename User::Identity const& identity);
      elle::Status      Create(typename Group::Identity const& identity);

      /// XXX
      Type
      type() const;
      /// XXX
      typename User::Identity const&
      user() const;
      /// XXX
      typename Group::Identity const&
      group() const;

      //
      // interfaces
      //

      // object
#include <elle/idiom/Open.hh>
      declare(Subject);
#include <elle/idiom/Close.hh>
      elle::Boolean     operator==(const Subject&) const;

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // serialize
      ELLE_SERIALIZE_FRIEND_FOR(Subject);

      //
      // attributes
      //
    private:
      Type _type;

      union
      {
        typename User::Identity* _user;
        typename Group::Identity* _group;
      };
    };

  }
}

# include <nucleus/neutron/Subject.hxx>

#endif
