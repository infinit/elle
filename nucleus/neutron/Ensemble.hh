#ifndef NUCLEUS_NEUTRON_ENSEMBLE_HH
# define NUCLEUS_NEUTRON_ENSEMBLE_HH

# include <elle/types.hh>
# include <elle/operator.hh>
# include <elle/attribute.hh>
# include <elle/serialize/Serializable.hh>

# include <cryptography/fwd.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Value.hh>
# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Range.hh>
# include <nucleus/neutron/Size.hh>
# include <nucleus/neutron/Fellow.hh>

namespace nucleus
{
  namespace neutron
  {
    /// An Ensemble block contains the list of the members of a
    /// group, known as the fellows.
    class Ensemble:
      public proton::Value,
      public elle::serialize::SerializableMixin<Ensemble>
    {
      /*------.
      | Types |
      `------*/
    public:
      typedef std::map<Subject const, std::shared_ptr<Fellow>> Container;
      typedef typename Container::iterator Iterator;
      typedef typename Container::const_iterator Scoutor;

      /*----------.
      | Constants |
      `----------*/
    public:
      struct Constants
      {
        static proton::Nature const seam;
        static proton::Nature const quill;
        static proton::Nature const value;
        static proton::Nature const nature;
      };

      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// Transfer a number of fellows from the _left_ ensemble to the _right_.
      /// The process stops when the _left_ ensemble is left with _size_ bytes
      /// of fellow i.e footprint.
      ///
      /// Note that the _right_ ensemble is supposed to contain higher subjects
      /// so that the fellows from the _left_ ensemble with the highest subjects
      /// are moved to _right_ in order to maintain consistency.
      static
      void
      transfer_right(Ensemble& left,
                     Ensemble& right,
                     proton::Extent const size);
      /// Does the same as transfer_right() by moving fellows from the _right_
      /// ensemble to the _left_.
      static
      void
      transfer_left(Ensemble& left,
                    Ensemble& right,
                    proton::Extent const size);

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Ensemble();
      Ensemble(Ensemble const&) = default;
      /// Load constructor. Note that the ensemble's footprint cannot be
      /// computed by this contructor and should therefore be manually set
      /// following the deserialization.
      ELLE_SERIALIZE_CONSTRUCT_DECLARE(Ensemble);

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Insert a fellow to the ensemble.
      ///
      /// Note that the ownership is transferred to the ensemble.
      void
      insert(Fellow* fellow);
      /// Insert the given fellow.
      void
      insert(std::shared_ptr<Fellow> const& fellow);
      /// Return true if the given subject belongs to the ensemble.
      elle::Boolean
      exist(Subject const& subject) const;
      /// XXX
      void
      update(Subject const& subject,
             Token const& token);
      /// Return a reference on the fellow associated with the given subject.
      Fellow const&
      locate(Subject const& subject) const;
      /// Return the index associated with the given subject.
      Index
      seek(Subject const& subject) const;
      /// Return a reference on the fellow associated with the given index.
      Fellow const&
      select(Index const& index) const;
      /// Return the range [index, index+ size[ from the ensemble.
      Range<Fellow>
      consult(Index const& index,
              Size const& size) const;
      /// Erase the given subject from the ensemble.
      void
      erase(Subject const& subject);
      /// Remove the fellow associated with the given subject and return it.
      ///
      /// This method is especially useful when moving fellows between
      /// ensembles.
      std::shared_ptr<Fellow>
      takeout(Subject const& subject);
      /// Return the number of fellows in the ensemble.
      Size
      size() const;
    private:
      /// Take the given fellow and inject it into the ensemble's container.
      ///
      /// Note that this method does not update the other ensemble properties
      /// (except the footprint) so as to be used by the deserializer as well.
      void
      _inject(std::shared_ptr<Fellow> const& fellow);
      /// Return an iterator on the fellow associated with the given subject.
      Scoutor
      _iterator(Subject const& subject) const;
      /// Return an iterator on the fellow associated with the given subject.
      Iterator
      _iterator(Subject const& subject);

      /*----------.
      | Operators |
      `----------*/
    public:
      ELLE_OPERATOR_NO_ASSIGNMENT(Ensemble);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 margin = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Ensemble);
      // value
      typedef Subject K;
      elle::Boolean
      empty() const;
      Subject
      mayor() const;
      proton::Capacity
      capacity() const;
      proton::Handle
      split();
      void
      merge(proton::Handle& other);
      // iterable
      Scoutor
      begin() const;
      Scoutor
      end() const;
      //private: // XXX[to reinstate]
      Iterator
      begin();
      Iterator
      end();

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE(Container, container);
    };

    namespace ensemble
    {
      /*----------.
      | Functions |
      `----------*/

      /// XXX
      void
      upgrade(proton::Porcupine<Ensemble>& porcupine,
              cryptography::PrivateKey const& pass_k);
    }
  }
}

# include <nucleus/neutron/Ensemble.hxx>

#endif
