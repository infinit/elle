#ifndef  ELLE_SERIALIZE_INSERT_HXX
# define ELLE_SERIALIZE_INSERT_HXX

# include "Exception.hh"

# include <iosfwd>
# include <memory>

namespace elle
{
  namespace serialize
  {

    /// @brief Input archive that owns a stream instance.
    template <typename Archive>
    class Insertor
    {
    protected:
      std::unique_ptr<std::ostream> _stream;
      std::unique_ptr<Archive>      _archive;

    public:
      explicit
      Insertor(std::unique_ptr<std::ostream>&& stream):
        _stream{std::move(stream)},
        _archive{new Archive{*this->_stream}}
      {
        if (!this->_stream->good())
          throw Exception("stream integrity is bad");
      }

      Insertor(Insertor&& other):
        _stream{std::move(other._stream)},
        _archive{std::move(other._archive)}
      {}

      template <typename T>
      Insertor&
      operator <<(T&& val)
      {
        (*_archive) << std::forward<T>(val);
        return *this;
      }

      template <typename T>
      Insertor&
      operator &(T&& val)
      {
        (*_archive) & std::forward<T>(val);
        return *this;
      }

      Insertor(Insertor const&) = delete;
      Insertor& operator =(Insertor const&) = delete;
      Insertor& operator =(Insertor&&) = delete;
    };

    namespace detail
    {
      void
      _flush_ostringstream(std::ostream& out,
                           std::string& str);
    }

    template <typename Archive>
    class StringInsertor
    {
    private:
      std::unique_ptr<std::ostream> _stream;
      std::unique_ptr<Archive>      _archive;
      std::string&                  _str;

    public:
      StringInsertor(std::unique_ptr<std::ostream>&& stream,
                     std::string& str):
        _stream{std::move(stream)},
        _archive{new Archive{*this->_stream}},
        _str(str)
      {}

      StringInsertor(StringInsertor&& other):
        _stream{std::move(other._stream)},
        _archive{std::move(other._archive)},
        _str(other._str)
      {}

      ~StringInsertor()
      {
        _archive.reset(); // ensure archive is flushed
        if (this->_stream)
          detail::_flush_ostringstream(*this->_stream, _str);
      }

      template <typename T>
      StringInsertor&
      operator <<(T&& val)
      {
        (*_archive) << std::forward<T>(val);
        return *this;
      }

      template <typename T>
      StringInsertor&
      operator &(T&& val)
      {
        (*_archive) & std::forward<T>(val);
        return *this;
      }
    };

    namespace detail
    {

      // Avoid including fstream and sstream
      std::unique_ptr<std::ostream>
      new_ofstream(std::string const& path);

      std::unique_ptr<std::ostream>
      new_ostringstream();

    }

    template <typename Archive>
    Insertor<Archive>
    to_file(std::string const& path)
    {
      return Insertor<Archive>{std::move(detail::new_ofstream(path))};
    }

    template <typename Archive>
    StringInsertor<Archive>
    to_string(std::string& str)
    {
      return StringInsertor<Archive>{
        std::move(detail::new_ostringstream()),
        str,
      };
    }

  }
}

#endif
