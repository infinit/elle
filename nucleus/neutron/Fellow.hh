#ifndef NUCLEUS_NEUTRON_FELLOW_HH
# define NUCLEUS_NEUTRON_FELLOW_HH

# include <elle/types.hh>
# include <elle/io/Dumpable.hh>

# include <nucleus/neutron/Subject.hh>
# include <nucleus/neutron/Token.hh>

namespace nucleus
{
  namespace neutron
  {

    /// XXX[represent a member in a group]
    class Fellow:
      public elle::io::Dumpable
    {
      //
      // types
      //
    public:
      typedef Subject S;

      //
      // constants
      //
    public:
      static const Fellow Null;

      //
      // construction
      //
    public:
      Fellow();
      Fellow(Subject const& subject);

      //
      // methods
      //
    public:
      /// XXX
      Subject const&
      subject() const;
      /// XXX
      Subject&
      subject();
      /// XXX
      Token const&
      token() const;
      /// XXX
      void
      token(Token const& token);

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 margin = 0) const;

      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Fellow);

      // rangeable
      Subject&
      Symbol();

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
