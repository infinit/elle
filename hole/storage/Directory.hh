#ifndef HOLE_STORAGE_DIRECTORY_HH
# define HOLE_STORAGE_DIRECTORY_HH

# include <elle/Printable.hh>
# include <elle/attribute.hh>

# include <hole/storage/Storage.hh>

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
      /// The overridden path implementation.
      virtual
      std::string
      _path(std::string const& identifier) const;
    };
  }
}

#endif
