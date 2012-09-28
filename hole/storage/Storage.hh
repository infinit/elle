#ifndef HOLE_STORAGE_STORAGE_HH
# define HOLE_STORAGE_STORAGE_HH

# include <string>

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
      exist(Address const& identifier,
            Revision const& revision = Revision::Last) const;
      /// The path to the file containing block at the given identifier.
      ///
      /// \throw if the block doesn't exist.
      /// \see exist
      std::string
      path(Address const& identifier,
           Revision const& revision = Revision::Last) const;
    private:
      std::string _identifier(Address const& addr,
                              Revision const& revision) const;

    protected:
      /// Implementation of exist to be overridden by subclasses.
      ///
      /// \see exist
      virtual
      bool
      _exist(std::string const& identifier) const = 0;
      /// Implementation of path to be overridden by subclasses.
      ///
      /// Existence is already ensured.
      /// \see path
      virtual
      std::string
      _path(std::string const& identifier) const = 0;
    };
  }
}

#endif
