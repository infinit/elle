//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        Alexandre Bique   [mon oct  3 16:04:02 2011]
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
                    size_t       input_size,
                    wchar_t **   output,
                    size_t *     output_size)
    {
      enter();
      escape("XXX implement me with iconv_open/iconv/iconv_close");
      leave();
    }

    Status Utf16To8(const wchar_t * input,
                    size_t          input_size,
                    char **         output,
                    size_t *        output_size)
    {
      enter();
      escape("XXX implement me with iconv_open/iconv/iconv_close");
      leave();
    }

  }
}
