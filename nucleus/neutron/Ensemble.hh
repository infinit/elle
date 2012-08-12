#ifndef NUCLEUS_NEUTRON_ENSEMBLE_HH
# define NUCLEUS_NEUTRON_ENSEMBLE_HH

# include <elle/types.hh>
# include <elle/concept/Serializable.hh>
# include <elle/serialize/BufferArchive.hh>
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
      public elle::concept::Serializable<Ensemble>,
      public elle::concept::Serializable<
        Ensemble,
        elle::serialize::BufferArchive
        >
    {
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
      Ensemble();

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
      update(elle::cryptography::PrivateKey const& pass);
      /// Removes the given subject from the ensemble.
      void
      remove(Subject const& subject);

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

      // serialize
      ELLE_SERIALIZE_FRIEND_FOR(Ensemble);

      ELLE_CONCEPT_SERIALIZABLE_METHODS(Ensemble);
      ELLE_CONCEPT_SERIALIZABLE_METHODS(Ensemble, elle::serialize::BufferArchive);

      //
      // attributes
      //
    private:
      Container _container;

      // XXX[add the owner's token, with a pointer]
    };

  }
}

# include <nucleus/neutron/Ensemble.hxx>

#endif
