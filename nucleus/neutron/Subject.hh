#ifndef NUCLEUS_NEUTRON_SUBJECT_HH
# define NUCLEUS_NEUTRON_SUBJECT_HH

# include <elle/types.hh>
# include <elle/radix/Object.hh>
# include <elle/cryptography/fwd.hh>

# include <nucleus/proton/fwd.hh>

namespace nucleus
{
  namespace neutron
  {

    ///
    /// this class is used to represents a subject i.e an entity which
    /// can be granted access such as a user or a group.
    ///
    class Subject
      : public elle::radix::Object
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
        Type            type;
        elle::String    name;
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
      Subject(const Subject&);
      ~Subject();

      //
      // methods
      //
      elle::Status      Create(elle::cryptography::PublicKey const&);
      elle::Status      Create(const proton::Address&);

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

      //
      // attributes
      //
      Type                      type;

      union
      {
        elle::cryptography::PublicKey*        user;
        proton::Address*        group;
      };
    };

  }
}

# include <nucleus/neutron/Subject.hxx>

#endif
