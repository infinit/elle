#ifndef NUCLEUS_NEUTRON_ENSEMBLE_HH
# define NUCLEUS_NEUTRON_ENSEMBLE_HH

# include <elle/types.hh>
# include <elle/serialize/Serializable.hh>
# include <elle/cryptography/fwd.hh>

# include <nucleus/proton/ContentHashBlock.hh>
# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Range.hh>
# include <nucleus/neutron/Fellow.hh>

namespace nucleus
{
  namespace neutron
  {

    /// An Ensemble block contains the list of the members of a
    /// group, known as the fellows.
    class Ensemble:
      public proton::ContentHashBlock,
      public elle::serialize::SerializableMixin<Ensemble>
    {
      //
      // constants
      //
    public:
      static const Component component;

      //
      // types
      //
    public:
      typedef std::list<Fellow*> Container;
      typedef typename Container::iterator Iterator;
      typedef typename Container::const_iterator Scoutor;

      //
      // construction
      //
    public:
      Ensemble(); // XXX[to deserialize]
      Ensemble(proton::Network const& network,
               elle::cryptography::PublicKey const& creator_K);

      //
      // methods
      //
    public:
      /// Adds a fellow to the ensemble. Note that the ownership is taken
      /// over the fellow.
      void
      add(std::unique_ptr<Fellow>&& fellow);
      /// Returns true if the ensemble is empty.
      elle::Boolean
      empty() const;
      /// Returns true if the given subject belongs to the ensemble.
      elle::Boolean
      exists(Subject const& subject) const;
      /// Returns a reference on the fellow associated with the given subject.
      Fellow const&
      locate(Subject const& subject) const;
      /// Returns a reference on the fellow associated with the given index.
      Fellow const&
      locate(Index const& index) const;
      /// Returns an index associated with the given subject.
      Index
      seek(Subject const& subject) const;
      /// Returns a Range of _size_ fellows from the ensemble, starting
      /// at _index_.
      Range<Fellow> const
      consult(Index const& index,
              Size const& size) const;
      /// Updates the tokens of the ensemble's fellows with the given
      /// pass private key.
      void
      update(elle::cryptography::PrivateKey const& pass_k);
      /// Removes the given subject from the ensemble.
      void
      remove(Subject const& subject);
      /// Returns the number of fellows in the ensemble.
      Size
      size() const;

    private:
      Scoutor const
      _locate(Subject const& subject) const;
      Iterator const
      _locate(Subject const& subject);

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 margin = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;

      // serialize
      ELLE_SERIALIZE_FRIEND_FOR(Ensemble);
      ELLE_SERIALIZE_SERIALIZABLE_METHODS(Ensemble);

      //
      // attributes
      //
    private:
      ELLE_ATTRIBUTE(Container, container);
    };

  }
}

# include <nucleus/neutron/Ensemble.hxx>

#endif
