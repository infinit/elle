#ifndef NUCLEUS_NEUTRON_ACCESS_HH
# define NUCLEUS_NEUTRON_ACCESS_HH

# include <elle/types.hh>
# include <elle/operator.hh>
# include <elle/attribute.hh>
# include <elle/serialize/Serializable.hh>
# include <elle/concept/Uniquable.hh>

# include <nucleus/proton/ContentHashBlock.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Value.hh>
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
      public proton::Value,
      public elle::serialize::SerializableMixin<Access>
    {
      /*------.
      | Types |
      `------*/
    public:
      typedef std::map<Subject const, std::shared_ptr<Record>> Container;
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
      /// Transfer a number of records from the _left_ access to the _right_.
      /// The process stops when the _left_ access is left with _size_ bytes of
      /// records i.e footprint.
      ///
      /// Note that the _right_ accesss is supposed to contain higher names
      /// so that the records from the _left_ access with the highest names
      /// are moved to _right_ in order to maintain consistency.
      static
      void
      transfer_right(Access& left,
                     Access& right,
                     proton::Extent const size);
      /// Does the same as transfer_right() by moving records from the _right_
      /// access to the _left_.
      static
      void
      transfer_left(Access& left,
                    Access& right,
                    proton::Extent const size);

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Access();
      Access(Access const&) = default;
      /// Load constructor. Note that the access's footprint cannot be
      /// computed by this contructor and should therefore be manually following
      /// the deserialization.
      ELLE_SERIALIZE_CONSTRUCT_DECLARE(Access);

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Insert a record in the access control list.
      ///
      /// Note that the given pointer's ownership is transferred to the
      /// access instance.
      void
      insert(Record* record);
      /// Insert the given record.
      void
      insert(std::shared_ptr<Record> const& record);
      /// Return true if a record exists for the given subject.
      elle::Boolean
      exist(Subject const& subject) const;
      /// XXX
      void
      update(Subject const& subject,
             Permissions const permissions);
      /// XXX
      void
      update(Subject const& subject,
             Token const& token);
      /// Return the record assocaited with the given subject.
      Record const&
      locate(Subject const& subject) const;
      /// Return the index of the record associated with the given subject.
      Index
      seek(Subject const& subject) const;
      /// Return the record located at the given index.
      Record const&
      select(Index const& index) const;
      /// Return the range [index, index + size[ from the access control list.
      Range<Record>
      consult(Index const& index,
              Size const& size) const;
      /// Erase the record associated with the given subject from the access
      /// control list.
      void
      erase(Subject const& subject);
      /// Remove the record associated with the given subject and return it.
      ///
      /// This method is especially useful when moving records between accesses.
      std::shared_ptr<Record>
      takeout(Subject const& subject);
      /// Return the number of records in the access control list.
      Size
      size() const;
    private:
      /// Take the given record and inject it into the access's container.
      ///
      /// Note that this method does not update the other access properties
      /// (except the footprint) so as to be used by the deserializer as well.
      void
      _inject(std::shared_ptr<Record> const& record);
      /// Return an iterator on the record associated with the given subject.
      Scoutor
      _iterator(Subject const& subject) const;
      /// Return an iterator on the record associated with the given subject.
      Iterator
      _iterator(Subject const& subject);

      /*----------.
      | Operators |
      `----------*/
    public:
      ELLE_OPERATOR_NO_ASSIGNMENT(Access);

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
    private:
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

    namespace access
    {
      /*----------.
      | Functions |
      `----------*/

      // Return the fingerprint of the access (subject, permissions) tuples.
      cryptography::Digest
      fingerprint(proton::Porcupine<Access> const& porcupine);
    }
  }
}

# include <nucleus/neutron/Access.hxx>

#endif
