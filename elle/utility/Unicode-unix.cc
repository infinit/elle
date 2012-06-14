#include <elle/types.hh>
#include <elle/standalone/Report.hh>
#include <elle/standalone/Log.hh>

#include <elle/idiom/Close.hh>
# include <iconv.h>
#include <elle/idiom/Open.hh>


namespace elle
{
  namespace utility
  {

    Status Utf8To16(const char*,
                    ssize_t,
                    wchar_t**,
                    ssize_t*)
    {
      escape("XXX implement me with iconv_open/iconv/iconv_close");
      return Status::Ok;
    }

    Status Utf16To8(const wchar_t*,
                    ssize_t,
                    char**,
                    ssize_t*)
    {
      escape("XXX implement me with iconv_open/iconv/iconv_close");
      return Status::Ok;
    }

  }
}
