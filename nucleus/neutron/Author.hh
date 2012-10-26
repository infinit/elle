#ifndef NUCLEUS_NEUTRON_AUTHOR_HH
# define NUCLEUS_NEUTRON_AUTHOR_HH

# include <elle/types.hh>
# include <elle/Printable.hh>

# include <nucleus/neutron/Object.hh>
# include <nucleus/neutron/Index.hh>

# include <elle/operator.hh>
# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace neutron
  {
    /// An object modifier.
    class Author:
      public elle::Printable
    {
    /*-------------.
    | Construction |
    `-------------*/
    public:
      /// Create a owner-specific Author.
      Author();
      /// Create a lord-specific Author, used whenever a user has been directly
      /// granted access to an object (i.e is explicitely listed in the Access
      /// block).
      Author(Index const& idx);

    /*----------.
    | Operators |
    `----------*/
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
      // printable
      virtual
      void
      print(std::ostream& stream) const;

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
