#ifndef NUCLEUS_NEUTRON_AUTHOR_HH
# define NUCLEUS_NEUTRON_AUTHOR_HH

# include <elle/types.hh>

# include <nucleus/neutron/Object.hh>
# include <nucleus/neutron/Index.hh>

# include <elle/operator.hh>
# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace neutron
  {

    ///
    /// this class represents the last user to have modified an object i.e
    /// the author.
    ///
    class Author
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
      // operators
      //
    public:
      elle::Boolean
      operator ==(Author const& other) const;
      ELLE_OPERATOR_NEQ(Author);
      ELLE_OPERATOR_ASSIGNMENT(Author);

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //

      Object::Role role;

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
