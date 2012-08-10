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

    /// XXX[represents the list of users in a group]
    /// XXX[needs to rely on Porcupine]
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
      /// XXX
      void
      add(std::unique_ptr<Fellow>&& fellow);
      /// XXX
      elle::Boolean
      empty() const;
      /// XXX
      elle::Boolean
      exists(Subject const& subject) const;
      /// XXX
      Fellow const*
      locate(Subject const& subject) const;
      /// XXX
      Fellow const*
      locate(Index const& index) const;
      /// XXX
      Index
      seek(Subject const& subject) const;
      /// XXX
      Range<Fellow> const
      consult(Index const& index,
              Size const& size) const;
      /// XXX
      void
      update(elle::cryptography::PrivateKey const& k);
      /// XXX
      void
      remove(Subject const& subject);

    private:
      /// XXX
      Scoutor const
      _locate(Subject const& subject) const;
      /// XXX
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
