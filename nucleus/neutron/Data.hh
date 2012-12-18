#ifndef NUCLEUS_NEUTRON_DATA_HH
# define NUCLEUS_NEUTRON_DATA_HH

# include <elle/types.hh>
# include <elle/operator.hh>
# include <elle/attribute.hh>
# include <elle/Buffer.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Capacity.hh>
# include <nucleus/proton/Handle.hh>
# include <nucleus/proton/Value.hh>
# include <nucleus/neutron/Offset.hh>
# include <nucleus/neutron/Size.hh>

namespace nucleus
{
  namespace neutron
  {
    ///
    /// this class represents a file's content.
    ///
    /// note that the Data does not derive the Block class. indeed, the
    /// Contents class represents the container for genre-specific content:
    /// Catalog for directories, Data for files etc.
    ///
    /// XXX rewrite: a noter qu'on ne peut pas inserer de contenu en mode
    ///     Data. donc le split est straighforward, comme le merge. ainsi
    ///     chaque Data represente une zone a un offset et l'offset ne change
    ///     jamais puisqu'on ne peut pas enlever/ajouter de contenu avant
    ///     la seule operation consistant a reduire/etendre la taille.
    ///
    class Data:
      public proton::Value,
      public elle::serialize::SerializableMixin<Data>
    {
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
      /// Transfer a number of bytes from the _left_ data to the _right_.
      /// The process stops when the _left_ data is left with _size_ bytes of
      /// content i.e footprint.
      ///
      /// Note that the _right_ data is supposed to contain higher offsets
      /// so that the content from the _left_ data with the highest offsets
      /// are moved to _right_ in order to maintain consistency.
      static
      void
      transfer_right(Data& left,
                     Data& right,
                     proton::Extent const size);
      /// Does the same as transfer_right() by moving content from the _right_
      /// data to the _left_.
      static
      void
      transfer_left(Data& left,
                    Data& right,
                    proton::Extent const size);

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Data();
      Data(Offset const& offset);
      Data(Data const& other);

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Write the data included in _buffer_ at the given relative offset
      /// _offset_ i.e the offset in the internal buffer (as opposed to
      /// the offset in the content which possibly spans over multiple Data
      /// nodes).
      void
      write(Offset const& offset,
            elle::WeakBuffer const& buffer);
      /// Return _size_ bytes of the data located at the given relative
      /// offset _offset_ i.e the offset within the internal buffer.
      elle::Buffer
      read(Offset const& offset,
           Size const& size) const;
      /// Set the new relative size of the data to _size_.
      void
      adjust(Size const& size);
      /// Return the size of the data.
      Size
      size() const;

      /*----------.
      | Operators |
      `----------*/
    public:
      ELLE_OPERATOR_NO_ASSIGNMENT(Data);

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
      ELLE_SERIALIZE_FRIEND_FOR(Data);
      ELLE_SERIALIZE_SERIALIZABLE_METHODS(Data);
      // value
      typedef Offset K;
      elle::Boolean
      empty() const;
      Offset
      mayor() const;
      proton::Capacity
      capacity() const;
      proton::Handle
      split();
      void
      merge(proton::Handle& other);

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      /// The absolute offset of the content zone which the buffer below
      /// represent.
      ELLE_ATTRIBUTE_RW(Offset, offset);
      /// The actual data buffer.
      ELLE_ATTRIBUTE(elle::Buffer, buffer);
    };
  }
}

# include <nucleus/neutron/Data.hxx>

#endif
