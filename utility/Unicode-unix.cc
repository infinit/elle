//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri jan 13 19:26:19 2012]
//

#include <elle/Elle.hh>

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
      return elle::StatusOk;
    }

    Status Utf16To8(const wchar_t * input,
                    ssize_t         input_size,
                    char **         output,
                    ssize_t *       output_size)
    {
      escape("XXX implement me with iconv_open/iconv/iconv_close");
      return elle::StatusOk;
    }

  }
}
