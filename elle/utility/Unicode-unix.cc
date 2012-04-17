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

    Status Utf8To16(const char * input,
                    ssize_t      input_size,
                    wchar_t **   output,
                    ssize_t *    output_size)
    {
      escape("XXX implement me with iconv_open/iconv/iconv_close");
      return Status::Ok;
    }

    Status Utf16To8(const wchar_t * input,
                    ssize_t         input_size,
                    char **         output,
                    ssize_t *       output_size)
    {
      escape("XXX implement me with iconv_open/iconv/iconv_close");
      return Status::Ok;
    }

  }
}
