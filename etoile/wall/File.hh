#ifndef ETOILE_WALL_FILE_HH
# define ETOILE_WALL_FILE_HH

# include <elle/types.hh>
# include <elle/fwd.hh>

# include <nucleus/neutron/fwd.hh>

# include <etoile/path/fwd.hh>
# include <etoile/gear/fwd.hh>

namespace etoile
{
  namespace wall
  {
    ///
    /// this class provides functionalities for managing file objects.
    ///
    class File
    {
      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// Create a new file object.
      ///
      /// Note however that the object is not attached to the hierarchy
      /// and is therefore considered as orphan.
      static
      gear::Identifier
      create();
      /// Load the file and returns an identifier for manipuling it.
      static
      gear::Identifier
      load(path::Chemin const& chemin);
      /// Write the file with the given region of data.
      static
      void
      write(gear::Identifier const& identifier,
            nucleus::neutron::Offset const& offset,
            elle::WeakBuffer const& data);
      /// Read _size_ bytes of data from the file, at the given offset
      /// _offset_.
      static
      elle::Buffer
      read(gear::Identifier const& idenifier,
           nucleus::neutron::Offset const& offset,
           nucleus::neutron::Size const& size);

      static elle::Status       Adjust(const gear::Identifier&,
                                       const nucleus::neutron::Size&);

      /// Discard the scope, potentially ignoring some modifications.
      static
      void
      discard(gear::Identifier const& identifier);
      /// Commit the pending modifications by placing the scope in the journal.
      static
      void
      store(gear::Identifier const& identifier);

      static elle::Status       Destroy(const gear::Identifier&);

      // XXX[to remove as soon as possible i.e when etoile will be instanciable]
      static
      nucleus::neutron::Size
      transferto(elle::String const& source,
                 gear::Identifier const& destination,
                 nucleus::neutron::Offset const& offset);
      static
      nucleus::neutron::Size
      transferfrom(gear::Identifier const& source,
                   elle::String const& destination,
                   nucleus::neutron::Offset const& offset,
                   nucleus::neutron::Size const& size);
    };
  }
}

#endif
