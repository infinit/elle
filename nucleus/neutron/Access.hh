#ifndef NUCLEUS_NEUTRON_ACCESS_HH
# define NUCLEUS_NEUTRON_ACCESS_HH

# include <elle/types.hh>
# include <elle/operator.hh>
# include <elle/serialize/Serializable.hh>

# include <nucleus/proton/ContentHashBlock.hh>

# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Range.hh>
# include <nucleus/neutron/Record.hh>

# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace neutron
  {

    /// This class represents an access control list.
    class Access:
      public proton::ContentHashBlock,
      public elle::serialize::SerializableMixin<Access>,
      public elle::concept::MakeUniquable<Access>
    {
      //
      // constants
      //
    public:
      static const Component component;

      //
      // construction
      //
    public:
      Access(); // XXX[to deserialize]
      ELLE_SERIALIZE_CONSTRUCT(Access, ContentHashBlock)
      {}
      Access(proton::Network const& network,
             elle::cryptography::PublicKey const& creator_K);

      //
      // methods
      //
    public:
      elle::Status      Add(Record*);
      elle::Status      Exist(const Subject&) const;
      elle::Status      Lookup(const Subject&,
                               Record const*&) const;
      elle::Status      Lookup(const Subject&,
                               Record*&) const;
      elle::Status      Lookup(const Subject&,
                               Index&) const; // XXX[rename to seek()]
      elle::Status      Lookup(const Index&,
                               Record*&) const;
      /// XXX
      template <typename T>
      elle::Status
      Update(Subject const& subject,
             Permissions permissions,
             T const& secret,
             elle::cryptography::PublicKey const& K);
      /// XXX
      elle::Status
      Update(Subject const& subject,
             Permissions permissions,
             Token const& token);
      elle::Status      Consult(const Index&,
                                const Size&,
                                Range<Record>&) const;
      elle::Status      Remove(const Subject&);
      elle::Status      Capacity(Size&) const;
      elle::Status      Fingerprint(elle::cryptography::Digest&) const;

      //
      // operators
      //
    public:
      elle::Boolean
      operator ==(Access const& other) const;
      ELLE_OPERATOR_NEQ(Access);

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
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Access);
      // fileable
      ELLE_SERIALIZE_SERIALIZABLE_METHODS(Access);
      // iterable
      typename Range<Record>::Scoutor
      begin() const;
      typename Range<Record>::Scoutor
      end() const;

      //
      // attributes
      //
    private:
      Range<Record> _range;
    };

  }
}

# include <nucleus/neutron/Access.hxx>

#endif
