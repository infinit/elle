#ifndef NUCLEUS_NEUTRON_ACCESS_HH
# define NUCLEUS_NEUTRON_ACCESS_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/serialize/Serializable.hh>
# include <elle/concept/Uniquable.hh>

# include <cryptography/oneway.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

# include <nucleus/proton/ContentHashBlock.hh>

# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Range.hh>
# include <nucleus/neutron/Record.hh>
# include <nucleus/neutron/Size.hh>

namespace nucleus
{
  namespace neutron
  {
    /// Represent an access control list which contains tuples of subjects
    /// (user or group) having been granted permssion on the object.
    class Access:
      public proton::ContentHashBlock,
      public elle::serialize::SerializableMixin<Access>,
      public elle::concept::UniquableMixin<Access>
    {
      /*----------.
      | Constants |
      `----------*/
    public:
      struct Constants
      {
        static cryptography::oneway::Algorithm const oneway_algorithm;

        static Component const component;
      };

      /*------.
      | Types |
      `------*/
    public:
      typedef std::map<Subject const, std::shared_ptr<Record>> Container;
      typedef typename Container::iterator Iterator;
      typedef typename Container::const_iterator Scoutor;

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Access(); // XXX[to deserialize]
      Access(proton::Network const& network,
             cryptography::PublicKey const& creator_K);
      ELLE_SERIALIZE_CONSTRUCT_DECLARE(Access);

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Insert a record in the access control list.
      ///
      /// Note that the given pointer's ownership is transfered to the
      /// Access instance.
      void
      insert(Record* record);
      /// Return true if a record exists for the given subject.
      elle::Boolean
      exist(Subject const& subject) const;
      /// Return the record assocaited with the given subject.
      Record const&
      locate(Subject const& subject) const;
      /// Return the record associated with the given subject.
      Record&
      locate(Subject const& subject);
      /// Return the index of the record associated with the given subject.
      Index
      seek(Subject const& subject) const;
      /// Return the record located at the given index.
      Record const&
      select(Index const& index) const;
      /// Return the record located at the given index.
      Record&
      select(Index const& index);
      /// Return the range [index, index + size[ from the access control list.
      Range<Record>
      consult(Index const& index,
              Size const& size) const;
      /// Erase the record associated with the given subject from the access
      /// control list.
      void
      erase(Subject const& subject);
      /// Return the size of the access control list.
      Size
      size() const;
      /// Compute a fingerprint of the record tuples composing the access block.
      cryptography::Digest
      fingerprint() const;

      // XXX[transition for etoile]
      Record const*
      lookup(Subject const& subject) const
      {
        Record const& record = this->locate(subject);
        return (&record);
      }
      Record*
      lookup(Subject const& subject)
      {
        Record& record = this->locate(subject);
        return (&record);
      }
      // XXX
    private:
      /// Insert a shared_ptr-based record.
      void
      _insert(std::shared_ptr<Record> const& record);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Access);
      // iterable
      Scoutor
      begin() const;
      Scoutor
      end() const;
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
  }
}

# include <nucleus/neutron/Access.hxx>

#endif
