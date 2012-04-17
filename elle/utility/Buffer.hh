#ifndef  ELLE_UTILITY_BUFFER_HH
# define ELLE_UTILITY_BUFFER_HH

# include <elle/types.hh>


namespace elle
{
  namespace utility
  {

    /// Manage a memory zone. Note that this class owns the pointed
    /// memory at every moment.
    ///
    /// @see WeakBuffer for a buffer that doesn't own the data
    class Buffer
    {
    private:
      elle::Byte*       _contents;
      size_t            _size;
      size_t            _buffer_size;

    public:
      Buffer();
      Buffer(size_t                 size);

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
      void Append(void const* data, size_t size);

      void Size(size_t size);
      size_t Size() const
        { return this->_size; }

    private:
      static size_t _NextSize(size_t);
    };

  }
}

#endif /* ! BUFFER_HH */


