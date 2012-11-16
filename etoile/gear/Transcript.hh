#ifndef ETOILE_GEAR_TRANSCRIPT_HH
# define ETOILE_GEAR_TRANSCRIPT_HH

# include <elle/types.hh>
# include <elle/Printable.hh>

# include <etoile/gear/Action.hh>

# include <nucleus/proton/Address.hh>

# include <elle/idiom/Close.hh>
#  include <vector>
# include <elle/idiom/Open.hh>

# include <boost/noncopyable.hpp>

namespace etoile
{
  namespace gear
  {

    /// This class represents a set of actions to be performed on the
    /// storage layer such as pushing a block, removing another one and so
    /// on.
    class Transcript:
      public elle::Printable,
      private boost::noncopyable
    {
      //
      // types
      //
      typedef std::vector<Action*> Container;
      typedef Container::iterator               Iterator;
      typedef Container::const_iterator         Scoutor;

      //
      // construction
      //
    public:
      ~Transcript();

      //
      // methods
      //
    public:
      /// Records the address/block tuple for insertion in the storage
      /// layer.
      void
      push(nucleus::proton::Address const& address,
           nucleus::proton::Block const* block);
      /// Records the block associated with the given address for removal
      /// from the storage layer.
      void
      wipe(nucleus::proton::Address const& address);

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
      // iterable
      Scoutor
      begin() const;
      Scoutor
      end() const;

      //
      // attributes
      //
    private:
      ELLE_ATTRIBUTE(Container, container);
    };

  }
}

#endif
