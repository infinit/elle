#ifndef  ELLE_UTILITY_BUFFER_HH
# define ELLE_UTILITY_BUFFER_HH

# include <memory>

# include <elle/types.hh>

# include <elle/io/Dumpable.hh>
# include <elle/serialize/Uniquable.hxx>

# include <elle/serialize/fwd.hh>


namespace elle
{
  namespace utility
  {

    namespace detail
    {
      struct MallocDeleter
      {
        void operator() (void* data);
      };
    }

    class WeakBuffer;

    /// Manage a memory zone. Note that this class owns the pointed
    /// memory at every moment.
    ///
    /// @see WeakBuffer for a buffer that doesn't own the data
    class Buffer
      : public elle::io::Dumpable
      , public elle::serialize::Uniquable<Buffer>
    {
      friend class elle::serialize::ArchiveSerializer<Buffer>;
    public:
      typedef elle::Byte                                          ContentType;
      typedef std::unique_ptr<ContentType, detail::MallocDeleter> ContentPtr;
      typedef std::pair<ContentPtr, size_t>                       ContentPair;
    private:
      elle::Byte*       _contents;
      size_t            _size;
      size_t            _buffer_size;

    public:
      Buffer();
      Buffer(size_t                 size);
      Buffer(ContentPair&&          pair);
      Buffer(elle::Byte const* data, size_t size);

      // Buffer class is moveable
      Buffer(Buffer&&               other);
      Buffer& operator =(Buffer&&   other);

      // Buffer class is Inheritable
      virtual ~Buffer();

    private:
      // Buffer is not copyable
      Buffer(Buffer const&);
      Buffer& operator =(Buffer const&);

    public:
      /// Add a copy of the data to the end of the buffer.
      void                Append(void const* data, size_t size);

      /// Properties for the size and the buffer contents
      void                Size(size_t size);
      size_t              Size() const { return this->_size; }
      elle::Byte const*   Contents() const { return this->_contents; }
      elle::Byte*         MutableContents() { return this->_contents; }

      /// Reset the size to zero.
      void                Reset() { this->Size(0); }

      /// XXX add methods to fill with zeros

      /// Release internal memory.
      ContentPair         Release();

      /// Binary serialization shorcut.
      elle::serialize::OutputBufferArchive  Writer();
      elle::serialize::InputBufferArchive   Reader() const;

      virtual Status    Dump(const Natural32 shift = 0) const;

    private:
      static size_t _NextSize(size_t);
    };


    class WeakBuffer
      : public elle::serialize::Uniquable<WeakBuffer>
    {
    private:
      elle::Byte const*   _contents;
      size_t              _size;

    public:
      WeakBuffer(void const* data, size_t size)
        : _contents(static_cast<elle::Byte const*>(data))
        , _size(size)
      {}

      WeakBuffer(Buffer const& buffer)
        : _contents(buffer.Contents())
        , _size(buffer.Size())
      {}

      WeakBuffer(WeakBuffer const& buffer)
        : _contents(buffer.Contents())
        , _size(buffer.Size())
      {}

      WeakBuffer(WeakBuffer&& other)
        : _contents(other._contents)
        , _size(other._size)
      {}

      size_t              Size() const        { return this->_size; }
      elle::Byte const*   Contents() const    { return this->_contents; }

      elle::serialize::InputBufferArchive  Reader() const;
    };

  }
}

#endif /* ! BUFFER_HH */


