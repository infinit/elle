#ifndef HOLE_STORAGE_STORAGE_HH
# define HOLE_STORAGE_STORAGE_HH

# include <string>

# include <nucleus/proton/ImmutableBlock.hh>
# include <nucleus/proton/MutableBlock.hh>
# include <nucleus/proton/Address.hh>
# include <nucleus/proton/Revision.hh>

namespace hole
{
  namespace storage
  {
    /// An abstract local block storage.
    ///
    /// For now, the assumption that blocks are stored as files is made.
    class Storage
    {
    /*------.
    | Types |
    `------*/
    public:
      typedef Storage Self;
      typedef nucleus::proton::Address Address;
      typedef nucleus::proton::Revision Revision;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      /// Create a Storage.
      Storage();
      /// Destroy a Storage.
      virtual
      ~Storage();

    /*------------.
    | Storage API |
    `------------*/
    public:
      /// Whether a block is stored for the given identifier.
      bool
      exist(Address const&  identifier,
            Revision const& revision = Revision::Last) const;
      /// Store an immutable block in the storage.
      //
      /// \throw if block already exists.
      void
      store(const nucleus::proton::Address&        address,
            const nucleus::proton::ImmutableBlock& block) const;
      /// Store a mutable block in the storage.
      ///
      /// \throw if block already exists.
      void
      store(const nucleus::proton::Address&      address,
            const nucleus::proton::MutableBlock& block) const;
      /// Retrieve a block.
      ///
      /// \throw if address doesn't match any block.
      std::unique_ptr<nucleus::proton::Block>
      load(nucleus::proton::Address const& address) const;
      /// Retrieve block at specific revision.
      ///
      /// \throw if address doesn't match any block.
      std::unique_ptr<nucleus::proton::Block>
      load(nucleus::proton::Address const&  address,
          nucleus::proton::Revision const& revision) const;
      /// Destroy a specific block.
      ///
      /// \throw if address doesn't match any block.
      void
      erase(nucleus::proton::Address const& address) const;


    protected:
      std::string _identifier(Address const& addr,
                              Revision const& revision = Revision::Last) const;

    /*----------.
    | Utilities |
    `----------*/
    public:
      /// Conflict checking between locally stored block at address and block.
      ///
      bool
      conflict(nucleus::proton::Address const& address,
               nucleus::proton::MutableBlock const& block) const;

    protected:
    /*------------.
    | To override |
    `------------*/
      /// Implementation of exist to be overridden by subclasses.
      ///
      /// \see exist
      virtual
      bool
      _exist(std::string const& identifier) const = 0;
      /// Implementation of store for ib to be overridden by subclasses.
      //
      /// \see store.
      virtual
      void
      _store(const nucleus::proton::Address& address,
             const nucleus::proton::ImmutableBlock& block) const = 0;
      /// Implementation of store for mb to be overridden by subclasses.
      ///
      /// \see store.
      virtual
      void
      _store(const nucleus::proton::Address& address,
             const nucleus::proton::MutableBlock& block) const = 0;
      /// Implementation of get to be overridden by subclasses.
      ///
      /// \set get.
      virtual
      std::unique_ptr<nucleus::proton::Block>
      _load(nucleus::proton::Address const& address) const = 0;
      /// Implementation of get to be overridden by subclasses.
      ///
      /// \set get.
      virtual
      std::unique_ptr<nucleus::proton::Block>
      _load(nucleus::proton::Address const& address,
          nucleus::proton::Revision const& revision) const = 0;
      /// Implementation of erase to be overriden by subclasses.
      ///
      /// \set erase.
      virtual
      void
      _erase(nucleus::proton::Address const& address) const = 0;
    };
  }
}

#endif
