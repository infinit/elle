#ifndef ELLE_SERIALIZE_HEXADECIMALARCHIVE_HH
# define ELLE_SERIALIZE_HEXADECIMALARCHIVE_HH

# include <cassert>

# include "BaseArchive.hxx"

# include "HexadecimalArchive.fwd.hh"

namespace elle { namespace serialize {

  template<> class HexadecimalArchive<ArchiveMode::Output> :
    public BaseArchive<ArchiveMode::Output, HexadecimalArchive<ArchiveMode::Output>>
  {
  protected:
    typedef BaseArchive<ArchiveMode::Output, HexadecimalArchive<ArchiveMode::Output>> BaseClass;
    friend class BaseClass::Access;

  private:
    static constexpr char const* _hexChars = "0123456789abcdef";

  private:
    std::string _temp;

  public:
    HexadecimalArchive(StreamType& stream) :
      BaseClass(stream)
    {}

    template<typename T>
    HexadecimalArchive(StreamType& stream, T& value) :
      BaseClass(stream)
    { *this << value; }

    void SaveBinary(void const* data, size_t size)
    {
      if (size == 0)
        return;
      assert(data != nullptr);
      this->_temp.resize(size * 2);

      size_t i = 0;
      unsigned char const* ptr = static_cast<unsigned char const*>(data);
      unsigned char const* end = ptr + size;
      while (ptr != end)
        {
          unsigned char c = *ptr++;
          this->_temp[i++] = _hexChars[(c >> 4) & 0xf];
          this->_temp[i++] = _hexChars[c & 0xf];
        }

      if (i > 0)
        BaseClass::SaveBinary(this->_temp.data(), i);
    }

  };

  template<> class HexadecimalArchive<ArchiveMode::Input> :
    public BaseArchive<ArchiveMode::Input, HexadecimalArchive<ArchiveMode::Input>>
  {
  protected:
    typedef BaseArchive<ArchiveMode::Input, HexadecimalArchive<ArchiveMode::Input>> BaseClass;
    friend class BaseClass::Access;

  public:
    HexadecimalArchive(StreamType& stream)
      : BaseClass(stream)
    {}

  protected:
    void LoadBinary(void* data, size_t size)
    {
      char* ptr = static_cast<char*>(data);
      size_t i = 0;
      for (size_t i = 0; i < size; ++i)
        {
          char hex[2];
          BaseClass::LoadBinary(hex, 2);

# define GET_VAL_FROM_HEX(c)                  \
    (c >= '0' && c <= '9' ? c - '0' :         \
     (c >= 'a' && c <= 'f' ? c - 'a' + 10 :   \
      -1))
          char v1 = GET_VAL_FROM_HEX(hex[0]);
          char v2 = GET_VAL_FROM_HEX(hex[1]);
# undef GET_VAL_FROM_HEX

          if (v1 == -1 || v2 == -1)
            throw std::runtime_error("Invalid char found in hexadecimal stream");

          ptr[i] = ((v1 << 4) & 0xf0) + (v2 & 0xf);
        }
    }
  };

}} // !ns elle::serialize

#endif /* ! HEXADECIMALARCHIVE_HH */


