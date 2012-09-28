#ifndef NUCLEUS_PROTON_MUTABLEBLOCK_HH
# define NUCLEUS_PROTON_MUTABLEBLOCK_HH

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Block.hh>
# include <nucleus/proton/Family.hh>
# include <nucleus/proton/Revision.hh>
# include <nucleus/proton/Base.hh>
# include <nucleus/neutron/Component.hh>

# include <elle/Printable.hh>

namespace nucleus
{
  namespace proton
  {

    ///
    /// this class abstracts the notion of mutable block, this difference
    /// with immutable blocks lying in the fact that the block's address
    /// is computed in a way that it does not change over time no matter
    /// how the content does.
    ///
    /// note that a revision number is included in order to distinguish the
    /// different revision of a block's history.
    ///
    /// a specific file ending with the '@' character links to the
    /// last revision of the mutable block. this way, the history does
    /// not have to be loaded which would incurr a huge performance overhead.
    ///
    /// noteworthy is that altough a symbolic link would have been perfect
    /// for this, the number of the last revision is manually stored in
    /// a regular file in order to be portable since Windows, for instance,
    /// does not support symbolic links.
    ///
    /// the _base_ attribute is used internally to keep a view of the
    /// block's original state i.e before being modified.
    ///
    class MutableBlock:
      public Block
    {
      //
      // constructors & destructors
      //
    public:
      MutableBlock(); /// XXX[to deserialization]
      MutableBlock(Network const network,
                   Family const family,
                   neutron::Component const component,
                   elle::cryptography::PublicKey const& creator_K);

      //
      // methods
      //
    public:
      /// Returns true if the instance derives the given
      /// mutable block i.e its revision is strictly higher than
      /// the given one's.
      elle::Boolean
      derives(MutableBlock const& other) const;

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
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(MutableBlock);
      // fileable
    public:
      ELLE_CONCEPT_FILEABLE_METHODS();

      //
      // attributes
      //
    private:
      ELLE_ATTRIBUTE_RW(Revision, revision);
      ELLE_ATTRIBUTE_RW(Base, base);
    };

  }
}

# include <nucleus/proton/MutableBlock.hxx>

#endif
