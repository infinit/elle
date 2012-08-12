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

    /// This class is used to represent a subject i.e an entity which
    /// can be granted access such as a user or a group.
    class Subject:
      public elle::radix::Object
    {
      //
      // enumerations
      //
    public:
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
    public:
      struct Descriptor
      {
        Type type;
        elle::String name;
      };

      //
      // static methods
      //
    public:
      /// Converts a string into a subject type.
      static
      elle::Status
      Convert(elle::String& const,
              Type&);
      /// Converts a subject type into a string.
      static
      elle::Status
      Convert(Type const,
              elle::String&);

      //
      // static attributes
      //
    private:
      /// This table maintains a mapping between subject types and their
      /// respective human-readable representations.
      static const Descriptor _descriptors[Types];

      //
      // construction
      //
    public:
      Subject();
      Subject(typename User::Identity const& identity);
      Subject(typename Group::Identity const& identity);
      Subject(Subject const& other);
      ~Subject();

      //
      // methods
      //
    public:
      /// XXX[to remove in favour of the constructor]
      elle::Status
      Create(typename User::Identity const& identity);
      /// XXX[to remove in favour of the constructor]
      elle::Status
      Create(typename Group::Identity const& identity);
      /// Returns the subject type: user or group.
      Type
      type() const;
      /// Returns the user's identity i.e public key.
      typename User::Identity const&
      user() const;
      /// Returns the group's identity i.e address.
      typename Group::Identity const&
      group() const;

      //
      // interfaces
      //
    public:
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
