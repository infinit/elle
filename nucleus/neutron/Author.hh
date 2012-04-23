#ifndef  NUCLEUS_NEUTRON_AUTHOR_HH
# define NUCLEUS_NEUTRON_AUTHOR_HH

# include <elle/types.hh>
# include <elle/radix/Object.hh>

# include <nucleus/neutron/Role.hh>
# include <nucleus/neutron/Index.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents the last user to have modified an object i.e
    /// the author.
    ///
    class Author:
      public elle::radix::Object
    {
    public:
      //
      // constants
      //
      static const Author               Null;

      //
      // constructors & destructors
      //
      Author();
      ~Author();

      //
      // methods
      //
      elle::Status      Create();
      elle::Status      Create(const Index&);

      //
      // interfaces
      //

      // object
      declare(Author);
      elle::Boolean     operator==(const Author&) const;

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // archivable
      //elle::Status      Serialize(elle::Archive&) const;
      //elle::Status      Extract(elle::Archive&);

      //
      // attributes
      //
      Role              role;

      union
      {
        struct
        {
          Index         index;
        }               lord;

        struct
        {
          // XXX
        }               vassal;
      };
    };

  }
}

#endif
