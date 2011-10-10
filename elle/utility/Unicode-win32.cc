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

# if INFINIT_WIN32
#  include <windows.h>
# endif

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
      enter();

      *output_size = MultiByteToWideChar(CP_UTF8, // CodePage
                                         0,       // dwFlags
                                         input,
                                         input_size,
                                         NULL,
                                         0);

      if (*output_size <= 0)
        escape("failed to convert from UTF-16 to UTF-8\n");

      *output = (wchar_t *)::realloc(*output, (1 + *output_size) * sizeof (wchar_t));
      if (!*output)
        escape("Out Of Memory!");

      MultiByteToWideChar(CP_UTF8, // CodePage
                          0,       // dwFlags
                          input,
                          input_size,
                          *output,
                          *output_size);
      output[*output_size] = L'\0';

      leave();
    }

    Status Utf16To8(const wchar_t * input,
                    ssize_t         input_size,
                    char **         output,
                    ssize_t *       output_size)
    {
      enter();

      *output_size = WideCharToMultiByte(CP_UTF8,    // CodePage
                                         0,          // dwFlags
                                         input,
                                         input_size, // cchWideChar
                                         NULL,       // lpMultiByteStr
                                         0,          // cbMultiByte
                                         NULL,       // lpDefaultChar
                                         NULL);      // lpUsedDefaultChar

      if (*output_size <= 0)
        escape("failed to convert from UTF-16 to UTF-8\n");

      *output = (char *)::realloc(*output, *output_size + 1);
      if (!*output)
        escape("Out Of Memory!");

      WideCharToMultiByte(CP_UTF8,      // CodePage
                          0,            // dwFlags
                          input,
                          input_size,   // cchWideChar
                          *output,      // lpMultiByteStr
                          *output_size, // cbMultiByte
                          NULL,         // lpDefaultChar
                          NULL);        // lpUsedDefaultChar
      output[*output_size] = '\0';

      leave();
    }

  }
}
