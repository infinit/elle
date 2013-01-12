#ifndef ETOILE_NEST_NEST_HH
# define ETOILE_NEST_NEST_HH

# include <elle/types.hh>
# include <elle/utility/Time.hh>

# include <etoile/nest/Pod.hh>
# include <etoile/gear/fwd.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Nest.hh>

# include <boost/noncopyable.hpp>

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
      public nucleus::proton::Nest,
      private boost::noncopyable
    {
      /*------.
      | Types |
      `------*/
    public:
      typedef std::map<nucleus::proton::Egg*, Pod*> Pods;
      typedef std::map<nucleus::proton::Address const, Pod*> Addresses;
      typedef std::map<elle::utility::Time, Pod*> Queue;

      /*-------------.
      | Construction |
      `-------------*/
    public:
      /// Construct a nest by providing the length of the secret key with which
      // the modified blocks will be encrypted.
      Nest(elle::Natural32 const secret_length,
           nucleus::proton::Limits const& limits);
      ~Nest();

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Transcribe the nest's state into a transcript representing the
      /// operations to perform on the storage layer: store a block, remove
      /// another one etc.
      gear::Transcript
      transcribe();
    private:
      /// Return true if the given address' block lies in the nest.
      elle::Boolean
      _exist(nucleus::proton::Address const& address) const;
      /// Insert the pod in the pods container.
      void
      _insert(Pod* pod);
      /// Create a mapping between an address and an egg.
      void
      _map(nucleus::proton::Address const& address,
           Pod* pod);
      /// Return the pod associated with the given egg.
      Pod*
      _lookup(std::shared_ptr<nucleus::proton::Egg> const& egg) const;
      /// Return the pod associated with the given address.
      Pod*
      _lookup(nucleus::proton::Address const& address) const;
      /// Remove the mapping for the given address.
      void
      _unmap(nucleus::proton::Address const& address);
      /// Try to optimize the nest according to internal limits and conditions.
      void
      _optimize();
      /// Load the block from the depot, if necessary, and set it in the egg.
      void
      _load(std::shared_ptr<nucleus::proton::Egg>& egg);

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
      /// The set of pods tracking the various content blocks.
      ///
      /// Note that this container is index by the address of the egg tracked
      /// by the pod so as to retrieve a pod based on an egg.
      ELLE_ATTRIBUTE(Pods, pods);
      /// Contain the addresses of the permanents blocks for which
      /// an egg exist in the nest.
      ELLE_ATTRIBUTE(Addresses, addresses);
      /// The LRU-sorted queue of pods.
      ELLE_ATTRIBUTE(Queue, queue);
      /// The length of the secret key with which the blocks having been
      /// created or modified will be encrypted.
      ELLE_ATTRIBUTE(elle::Natural32, secret_length);
    };
  }
}

#endif
