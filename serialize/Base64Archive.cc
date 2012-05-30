
#include <sstream>

#include "Base64Archive.hh"

using namespace elle::serialize;

char const* Base64Archive<ArchiveMode::Output>::_base64Chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/"
  ;

Base64Archive<ArchiveMode::Output>::Base64Archive(StreamType& stream)
  : BaseClass(stream)
{}

Base64Archive<ArchiveMode::Output>::~Base64Archive()
{
  unsigned char c1, c2;
  char res[4];
  switch (_trailingChars.size())
    {
  case 2:
    c1 = _trailingChars[0];
    c2 = _trailingChars[1];
    res[0] = _base64Chars[(c1 & 0xfc) >> 2];
    res[1] = _base64Chars[((c1 & 0x03) << 4) + ((c2 & 0xf0) >> 4)];
    res[2] = _base64Chars[(c2 & 0x0f) << 2];
    res[3] = '=';
    break;
  case 1:
    c1 = _trailingChars[0];
    res[0] = _base64Chars[(c1 & 0xfc) >> 2];
    res[1] = _base64Chars[(c1 & 0x03) << 4];
    res[2] = '=';
    res[3] = '=';
    break;
  case 0:
    return;
  default:
    assert(false && "Never reached");
    }
  assert(_trailingChars.size() > 0);
  BaseClass::SaveBinary(res, 4);
}

void Base64Archive<ArchiveMode::Output>::SaveBinary(void const* data, size_t size)
{
  if (size == 0)
    return;
  assert(data != nullptr);

  size_t i = 0;
  unsigned char const* ptr = static_cast<unsigned char const*>(data);

  this->_temp.resize(size + (size / 2) + 4); // 4/3*size is really needed
  unsigned char c1;
  unsigned char c2;
  unsigned char c3;

  if (_trailingChars.size() > 0)
    {
      if (size + _trailingChars.size() < 3)
        {
          assert(size == 1);
          assert(_trailingChars.size() == 1);
          _trailingChars.push_back(*ptr);
          return;
        }
      c1 = _trailingChars[0];
      if (_trailingChars.size() > 1)
        {
          assert(_trailingChars.size() == 2);
          c2 = _trailingChars[1];
          c3 = *ptr++;
          size -= 1;
        }
      else
        {
          c2 = *ptr++;
          c3 = *ptr++;
          size -= 2;
        }
      _trailingChars.clear();
      this->_temp[i++] = _base64Chars[(c1 & 0xfc) >> 2];
      this->_temp[i++] = _base64Chars[((c1 & 0x03) << 4) + ((c2 & 0xf0) >> 4)];
      this->_temp[i++] = _base64Chars[((c2 & 0x0f) << 2) + ((c3 & 0xc0) >> 6)];
      this->_temp[i++] = _base64Chars[c3 & 0x3f];
    }


  while (size >= 3)
    {
      c1 = *ptr++;
      c2 = *ptr++;
      c3 = *ptr++;
      size -= 3;
      this->_temp[i++] = _base64Chars[(c1 & 0xfc) >> 2];
      this->_temp[i++] = _base64Chars[((c1 & 0x03) << 4) + ((c2 & 0xf0) >> 4)];
      this->_temp[i++] = _base64Chars[((c2 & 0x0f) << 2) + ((c3 & 0xc0) >> 6)];
      this->_temp[i++] = _base64Chars[c3 & 0x3f];
    }

  switch (size)
    {
    case 2:
      _trailingChars.push_back(*ptr++);
      /* no break */
    case 1:
      _trailingChars.push_back(*ptr++);
      /* no break */
    case 0:
      break;
    default:
      assert(false && "Never reached");
      break;
    }

  if (i > 0)
    BaseClass::SaveBinary(this->_temp.data(), i);
}


Base64Archive<ArchiveMode::Input>::Base64Archive(StreamType& stream)
  : BaseClass(stream)
{}

void Base64Archive<ArchiveMode::Input>::LoadBinary(void* data, size_t size)
{
  size_t i = 0;
  if (_dataLeft.size())
    {
      size_t toCopy = (size > _dataLeft.size() ? _dataLeft.size() : size);
      _dataLeft.copy(static_cast<char*>(data), toCopy);
      _dataLeft.erase(0, toCopy);
      size -= toCopy;
      i += toCopy;
    }

  unsigned char from[4];
  while (size >= 3)
    {
      BaseClass::LoadBinary(from, 4);
      from[0] = _DecodeChar(from[0]);
      from[1] = _DecodeChar(from[1]);
      from[2] = _DecodeChar(from[2]);
      from[3] = _DecodeChar(from[3]);

      static_cast<unsigned char*>(data)[i++] = (from[0] << 2) + ((from[1] & 0x30) >> 4);
      static_cast<unsigned char*>(data)[i++] = ((from[1] & 0xf) << 4) + ((from[2] & 0x3c) >> 2);
      static_cast<unsigned char*>(data)[i++] = ((from[2] & 0x3) << 6) + from[3];
      size -= 3;
    }

  if (size)
    {
      BaseClass::LoadBinary(from, 4);
      from[0] = _DecodeChar(from[0]);
      from[1] = _DecodeChar(from[1]);
      from[2] = _DecodeChar(from[2]);
      from[3] = _DecodeChar(from[3]);
      switch (size)
        {
        case 2:
          _dataLeft.resize(1);
          static_cast<unsigned char*>(data)[i++] = (from[0] << 2) + ((from[1] & 0x30) >> 4);
          static_cast<unsigned char*>(data)[i++] = ((from[1] & 0xf) << 4) + ((from[2] & 0x3c) >> 2);
          _dataLeft[0] = ((from[2] & 0x3) << 6) + from[3];
          break;
        case 1:
          _dataLeft.resize(2);
          static_cast<unsigned char*>(data)[i++] = (from[0] << 2) + ((from[1] & 0x30) >> 4);
          _dataLeft[0] = ((from[1] & 0xf) << 4) + ((from[2] & 0x3c) >> 2);
          _dataLeft[1] = ((from[2] & 0x3) << 6) + from[3];
          break;
        default:
          assert(false && "Never reached");
        }
    }
}

char Base64Archive<ArchiveMode::Input>::_DecodeChar(unsigned char c)
{
  static int8_t const reverse[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,
    52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,-1,0,1,2,3,4,5,6,7,8,9,
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,-1,26,27,
    28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
  };

  if (c == '=')
    return 0;
  char res = reverse[c];
  if (res < 0)
    {
      std::stringstream ss;
      ss << '\'' << (char)c << "' (" << (unsigned int) c << ')';
      throw std::runtime_error("Unexpected base64 char: " + ss.str());
    }
  return res;
}
