#ifndef NUCLEUS_NEUTRON_AUTHOR_HH
# define NUCLEUS_NEUTRON_AUTHOR_HH

# include <elle/types.hh>
# include <elle/radix/Object.hh>

# include <nucleus/neutron/Object.hh>
# include <nucleus/neutron/Index.hh>

# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace neutron
  {

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

      //
      // attributes
      //

      // XXX[nucleus::neutron:: can be removed when Author no longer derive
      //     radix::Object]
      nucleus::neutron::Object::Role role;

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

# include <nucleus/neutron/Author.hxx>

#endif
