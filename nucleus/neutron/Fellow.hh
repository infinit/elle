#ifndef NUCLEUS_NEUTRON_FELLOW_HH
# define NUCLEUS_NEUTRON_FELLOW_HH

# include <elle/types.hh>
# include <elle/Printable.hh>
# include <elle/io/Dumpable.hh>

# include <nucleus/neutron/Subject.hh>
# include <nucleus/neutron/Token.hh>

namespace nucleus
{
  namespace neutron
  {

    /// This class represents a group member and is defined by
    /// a subject (which can be either a user or a group i.e a subgroup),
    /// and a token which is the encrypted version of the group's private
    /// pass.
    class Fellow:
      public elle::Printable,
      public elle::io::Dumpable
    {
      //
      // types
      //
    public:
      /// This type is used by the Range class for determining the type of
      /// the items' differenciable elements i.e kind of keys.
      typedef Subject Symbol;

      //
      // constants
      //
    public:
      /// This constant defines a null/empty/unused fellow.
      static const Fellow Null;

      //
      // construction
      //
    public:
      Fellow();
      Fellow(Subject const& subject);
      Fellow(Subject const& subject,
             Token const& token);

      //
      // methods
      //
    public:
      /// Returns the subject.
      Subject const&
      subject() const;
      /// Returns the subject.
      Subject&
      subject();
      /// Returns the token.
      Token const&
      token() const;
      /// Specifies a new token.
      void
      token(Token const& token);

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 margin = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Fellow);
      // rangeable
      Subject&
      symbol();

      //
      // attributes
      //
    private:
      Subject _subject;
      Token _token;
    };

  }
}

# include <nucleus/neutron/Fellow.hxx>

#endif
