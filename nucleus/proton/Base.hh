#ifndef NUCLEUS_PROTON_BASE_HH
# define NUCLEUS_PROTON_BASE_HH

# include <elle/types.hh>
# include <elle/cryptography/Digest.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Revision.hh>

# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

    ///
    /// a base references a precise revision of a mutable block.
    ///
    /// this construct is useful to make sure a mutable block derives
    /// from another one, and another one down to the original mutable
    /// block whose ownership can usually be statically verified.
    ///
    /// by following this chain, one can make sure a mutable block lies
    /// in the legitimate block's history, in other words, branches have
    /// not been author .
    ///
    class Base
    {
      //
      // construction
      //
    public:
      Base();
      Base(MutableBlock const& block);

      //
      // methods
      //
    public:
      elle::Status      Create(const Revision&,
                               elle::cryptography::Digest const&);

      elle::Status      Match(const MutableBlock&) const;

      //
      // operators
      //
    public:
      elle::Boolean
      operator==(Base const&) const;

      //
      // interfaces
      //
    public:
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Base);

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
    private:
      Revision _revision;
      elle::cryptography::Digest _digest;
    };

  }
}

# include <nucleus/proton/Base.hxx>

#endif
