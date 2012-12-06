#ifndef HOLE_STORAGE_MEMORY_HH
# define HOLE_STORAGE_MEMORY_HH

# include <elle/Printable.hh>
# include <elle/attribute.hh>

# include <hole/storage/Storage.hh>

# include <nucleus/proton/ImmutableBlock.hh>
# include <nucleus/proton/MutableBlock.hh>

# include <map>

namespace hole
{
  namespace storage
  {
    /// Storage that holds blocks in a map located in main memory.
    class Memory:
      public Storage,
      public elle::Printable
    {
      /*------.
      | Types |
      `------*/
    private:
      typedef std::map<elle::String const, elle::String const> Container;
      typedef typename Container::iterator Iterator;
      typedef typename Container::const_iterator Scoutor;

      /*-------------.
      | Construction |
      `-------------*/
    public:
      /// Create a main-memory-basedstorage.
      Memory();
      /// Delete a directory.
      ~Memory();
    private:
      ELLE_ATTRIBUTE(Container, container);

    /*----------.
    | Printable |
    `----------*/
    public:
      // XXX[to remove?]
      void
      dump() const;
      virtual
      void
      print(std::ostream& stream) const;

    /*--------.
    | Storage |
    `--------*/
    protected:
      /// The overridden exist implementation.
      virtual
      bool
      _exist(std::string const& identifier) const;
      /// The overriden store implementation.
      virtual
      void
      _store(const nucleus::proton::Address& address,
             const nucleus::proton::ImmutableBlock& block);
      /// The overriden store implementation.
      virtual
      void
      _store(const nucleus::proton::Address& address,
             const nucleus::proton::MutableBlock& block);
      /// The overriden load implementation.
      virtual
      std::unique_ptr<nucleus::proton::Block>
      _load(nucleus::proton::Address const& address) const;
      /// The overriden load implementation.
      virtual
      std::unique_ptr<nucleus::proton::Block>
      _load(nucleus::proton::Address const& address,
          nucleus::proton::Revision const& revision) const;
      /// The overriden erase implementation.
      virtual
      void
      _erase(nucleus::proton::Address const& address);
    };
  }
}

#endif
