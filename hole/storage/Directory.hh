#ifndef HOLE_STORAGE_DIRECTORY_HH
# define HOLE_STORAGE_DIRECTORY_HH

# include <elle/Printable.hh>
# include <elle/attribute.hh>

# include <hole/storage/Storage.hh>

# include <nucleus/proton/ImmutableBlock.hh>
# include <nucleus/proton/MutableBlock.hh>

namespace hole
{
  namespace storage
  {
    /// Storage that holds blocks as single files in a given
    /// directory.
    class Directory:
      public Storage,
      public elle::Printable
    {
    /*-------------.
    | Construction |
    `-------------*/
    public:
      /// Create a Directory storage.
      ///
      /// @param path The directory where to stock blocks.
      Directory(std::string const& root);
      /// Delete a directory.
      ~Directory();
    private:
      ELLE_ATTRIBUTE_R(std::string, root);

    /*----------.
    | Printable |
    `----------*/
    public:
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
             const nucleus::proton::ImmutableBlock& block) const;
      /// The overriden store implementation.
      virtual
      void
      _store(const nucleus::proton::Address& address,
             const nucleus::proton::MutableBlock& block) const;
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
      _erase(nucleus::proton::Address const& address) const;

    private:
      /*----------.
      | Utilities |
      `----------*/
      std::string
      path(nucleus::proton::Address const& address,
           nucleus::proton::Revision const& revision = nucleus::proton::Revision::Last) const;

      std::string
      path(std::string const& identifier) const;
    };
  }
}

#endif
