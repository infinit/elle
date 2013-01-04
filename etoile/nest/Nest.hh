#ifndef ETOILE_NEST_NEST_HH
# define ETOILE_NEST_NEST_HH

# include <elle/types.hh>

# include <etoile/gear/fwd.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Placement.hh>
# include <nucleus/proton/Nest.hh>

# include <set>
# include <map>

namespace etoile
{
  namespace nest
  {
    /// Provide a nest implementation which keeps content blocks in main memory.
    ///
    /// However, should a threshold be reached, the nest would pick the least
    /// recently used blocks and pre-published them onto the storage layer.
    class Nest:
      public nucleus::proton::Nest
    {
      /*------.
      | Types |
      `------*/
    public:
      typedef std::set<nucleus::proton::Egg*> Eggs;
      typedef std::map<nucleus::proton::Address const,
                       std::shared_ptr<nucleus::proton::Egg>> Permanents;

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Nest(nucleus::proton::Limits const& limits);
      ~Nest();

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Return true if the given address' block lies in the nest.
      elle::Boolean
      exists(nucleus::proton::Address const& address) const;
      /// Transcribe the nest's state into a transcript representing the
      /// operations to perform on the storage layer: store a block, remove
      /// another one etc.
      gear::Transcript
      transcribe();
    private:
      /// XXX
      void
      _insert(nucleus::proton::Placement const& placement,
              Pod* pod);
      /// XXX
      void
      _insert(nucleus::proton::Placement const& placement,
              nucleus::proton::Address const& address,
              Pod* pod);
      /// XXX
      Pod*
      _retrieve(nucleus::proton::Placement const& placement) const;
      /// XXX
      Pod*
      _retrieve(nucleus::proton::Address const& address) const;
      /// XXX
      void
      _delete(nucleus::proton::Placement const& placement);
      /// XXX
      void
      _delete(nucleus::proton::Address const& address);
      /// XXX
      void
      _optimize();

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // nest
      nucleus::proton::Handle
      attach(nucleus::proton::Contents* block);
      void
      detach(nucleus::proton::Handle& handle);
      void
      load(nucleus::proton::Handle& handle);
      void
      unload(nucleus::proton::Handle& handle);

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE(Eggs, eggs);
      ELLE_ATTRIBUTE(Permanents, permanents);
    };

  }
}

#endif
