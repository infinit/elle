#ifndef NUCLEUS_NEUTRON_ATTRIBUTES_HH
# define NUCLEUS_NEUTRON_ATTRIBUTES_HH

# include <elle/types.hh>
# include <elle/operator.hh>
# include <elle/attribute.hh>
# include <elle/serialize/construct.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Handle.hh>
# include <nucleus/proton/Capacity.hh>
# include <nucleus/proton/Value.hh>
# include <nucleus/neutron/fwd.hh>

namespace nucleus
{
  namespace neutron
  {
    /// Represent the extended attributes associated with an Object
    /// block or else.
    class Attributes:
      public proton::Value,
      public elle::serialize::SerializableMixin<Attributes>
    {
      /*------.
      | Types |
      `------*/
    public:
      typedef std::map<elle::String const, std::shared_ptr<Trait>> Container;
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
      /// Transfer a number of traits from the _left_ attributes to the _right_.
      /// The process stops when the _left_ attributes is left with _size_ bytes
      /// of traits i.e footprint.
      ///
      /// Note that the _right_ attributes is supposed to contain higher names
      /// so that the traits from the _left_ attributes with the highest names
      /// are moved to _right_ in order to maintain consistency.
      static
      void
      transfer_right(Attributes& left,
                     Attributes& right,
                     proton::Extent const size);
      /// Does the same as transfer_right() by moving traits from the _right_
      /// attributes to the _left_.
      static
      void
      transfer_left(Attributes& left,
                    Attributes& right,
                    proton::Extent const size);

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Attributes();
      Attributes(Attributes const&) = default;
      /// Load constructor. Note that the attributes' footprint cannot be
      /// computed by this contructor and should therefore be manually following
      /// the deserialization.
      ELLE_SERIALIZE_CONSTRUCT_DECLARE(Attributes);

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Insert the given trait in the attributes.
      ///
      /// Note that the ownership of the given trait is transferred to the
      /// attributes which will therefore be responsible for deleting it.
      void
      insert(Trait* trait);
      /// Insert the given trait and update accordingly the attributes's
      /// capacity and state.
      void
      insert(std::shared_ptr<Trait> const& trait);
      /// Return true if the attributes instance contains a trait with
      /// the given name.
      elle::Boolean
      exist(elle::String const& name) const;
      /// Rename an trait.
      void
      rename(elle::String const& from,
             elle::String const& to);
      /// Update the trait's value.
      void
      update(elle::String const& name,
             elle::String const& value);
      /// Return the trait associated with the given name.
      Trait const&
      locate(elle::String const& name) const;
      /// Return a subset of the attributes i.e [index, index + size[.
      ///
      /// Note that the index is relative to the attributes.
      Range<Trait>
      consult(Index const& index,
              Size const& size) const;
      /// Remove and release the trait associated with the given name from
      /// the attributes.
      void
      erase(elle::String const& name);
      /// Remove the trait associated with the given name and return the trait.
      ///
      /// This method is especially useful when moving traits between
      /// attributess.
      std::shared_ptr<Trait>
      takeout(elle::String const& name);
      /// Return the size of the attributes i.e the number of traits.
      Size
      size() const;
    private:
      /// Take the given trait and inject it into the attributes's container.
      ///
      /// Note that this method does not update the other properties (except
      /// the footprint) so as to be used by the deserializer as well.
      void
      _inject(std::shared_ptr<Trait> const& trait);
      /// Return an iterator on the trait associated with the given name.
      Scoutor
      _iterator(elle::String const& name) const;
      /// Return an iterator on the trait associated with the given name.
      Iterator
      _iterator(elle::String const& name);

      /*----------.
      | Operators |
      `----------*/
    public:
      ELLE_OPERATOR_NO_ASSIGNMENT(Attributes);

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
      // serialize
      ELLE_SERIALIZE_FRIEND_FOR(Attributes);
      ELLE_SERIALIZE_SERIALIZABLE_METHODS(Attributes);
      // value
      typedef elle::String K;
      elle::Boolean
      empty() const;
      elle::String
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
  }
}

# include <nucleus/neutron/Attributes.hxx>

#endif
