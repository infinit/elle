#ifndef NUCLEUS_PROTON_MUTABLEBLOCK_HH
# define NUCLEUS_PROTON_MUTABLEBLOCK_HH

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Block.hh>
# include <nucleus/proton/Family.hh>
# include <nucleus/proton/Version.hh>
# include <nucleus/proton/Base.hh>
# include <nucleus/neutron/Component.hh>

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
    /// note that a version number is included in order to distinguish the
    /// different version of a block's history.
    ///
    /// a specific file ending with the '@' character links to the
    /// last version of the mutable block. this way, the history does
    /// not have to be loaded which would incurr a huge performance overhead.
    ///
    /// noteworthy is that altough a symbolic link would have been perfect
    /// for this, the number of the last version is manually stored in
    /// a regular file in order to be portable since Windows, for instance,
    /// does not support symbolic links.
    ///
    /// the _base_ attribute is used internally to keep a view of the
    /// block's original state i.e before being modified.
    ///
    class MutableBlock:
      public Block
    {
    public:
      //
      // constructors & destructors
      //
      MutableBlock();
      MutableBlock(const Family,
                   const neutron::Component);

      //
      // methods
      //
    public:
      /// Returns true if the instance derives the given
      /// mutable block i.e its version is strictly higher than
      /// the given one's.
      elle::Boolean
      derives(MutableBlock const& other) const;

    private:
      /// XXX
      static
      elle::io::Path
      _path(Network const& network,
            Address const& address);
      /// XXX
      static
      elle::io::Path
      _path(Network const& network,
            Address const& address,
            elle::String const& version);

      //
      // interfaces
      //
    public:
      // object
#include <elle/idiom/Open.hh>
      declare(MutableBlock);
#include <elle/idiom/Close.hh>


      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // fileable
    protected:
      ELLE_CONCEPT_FILEABLE_METHODS();
    public:
      void
      load(Network const& network,
           Address const& address,
           Version const& version);
      void
      store(Network const& network,
            Address const& address) const;
      static
      void
      erase(Network const& network,
            Address const& address);
      static
      elle::Boolean
      exists(Network const& network,
             Address const& address,
             Version const& version = Version::Any);

      //
      // attributes
      //
      Version           version;

      Base              base;
    };

  }
}

# include <nucleus/proton/MutableBlock.hxx>

#endif
