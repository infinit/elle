#include <elle/Exception.hh>
#include <elle/types.hh>
#include <elle/utility/Unicode.hh>

#include <iconv.h>

namespace elle
{
  namespace utility
  {

    Status Utf8To16(const char*,
                    ssize_t,
                    wchar_t**,
                    ssize_t*)
    {
      throw Exception("XXX implement me with iconv_open/iconv/iconv_close");
      return Status::Ok;
    }

    Status Utf16To8(const wchar_t*,
                    ssize_t,
                    char**,
                    ssize_t*)
    {
      throw Exception("XXX implement me with iconv_open/iconv/iconv_close");
      return Status::Ok;
    }

  }
}
