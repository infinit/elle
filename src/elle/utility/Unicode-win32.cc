#include <elle/types.hh>

#include <windows.h>

namespace elle
{
  namespace utility
  {

    Status Utf8To16(const char * input,
                    ssize_t      input_size,
                    wchar_t **   output,
                    ssize_t *    output_size)
    {
      *output_size = MultiByteToWideChar(CP_UTF8, // CodePage
                                         0,       // dwFlags
                                         input,
                                         input_size,
                                         nullptr,
                                         0);

      if (*output_size <= 0)
        throw Exception("failed to convert from UTF-16 to UTF-8\n");

      *output = (wchar_t *)::realloc(*output, (1 + *output_size) * sizeof (wchar_t));
      if (!*output)
        throw Exception("Out Of Memory!");

      auto nchars = MultiByteToWideChar(CP_UTF8, // CodePage
                                        0,       // dwFlags
                                        input,
                                        input_size,
                                        *output,
                                        *output_size);
      (*output)[*output_size] = L'\0';

      if (nchars != *output_size)
        throw Exception("failed to convert string");

      return Status::Ok;
    }

    Status Utf16To8(const wchar_t * input,
                    ssize_t         input_size,
                    char **         output,
                    ssize_t *       output_size)
    {
      *output_size = WideCharToMultiByte(CP_UTF8,    // CodePage
                                         0,          // dwFlags
                                         input,
                                         input_size, // cchWideChar
                                         nullptr,       // lpMultiByteStr
                                         0,          // cbMultiByte
                                         nullptr,       // lpDefaultChar
                                         nullptr);      // lpUsedDefaultChar

      if (*output_size <= 0)
        throw Exception("failed to convert from UTF-16 to UTF-8\n");

      *output = (char *)::realloc(*output, *output_size + 1);
      if (!*output)
        throw Exception("Out Of Memory!");

      auto nchars = WideCharToMultiByte(CP_UTF8,      // CodePage
                                        0,            // dwFlags
                                        input,
                                        input_size,   // cchWideChar
                                        *output,      // lpMultiByteStr
                                        *output_size, // cbMultiByte
                                        nullptr,         // lpDefaultChar
                                        nullptr);        // lpUsedDefaultChar
      (*output)[*output_size] = '\0';

      if (nchars != *output_size)
        throw Exception("failed to convert string");

      return Status::Ok;
    }

  }
}
