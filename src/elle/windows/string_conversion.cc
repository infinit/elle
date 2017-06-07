#include <boost/locale.hpp>
#include <elle/windows/string_conversion.hh>
#include <elle/windows.hh>

namespace elle
{
  namespace string
  {
    // Shamelessly stolen from:
    // http://boost.2283326.n4.nabble.com/locale-filesystem-Windows-local-8-bit-encoding-td4637833.html
    static
    std::string
    windows_codepage_to_encoding(int codepage)
    {
      switch (codepage)
      {
        case 874: return "windows-874";

        case 932: return "Shift_JIS"; // but should be "Windows-31J";
        case 936: return "GB2312";
        case 949: return "KS_C_5601-1987";
        case 950: return "Big5";

        case 1250: return "windows-1250";
        case 1251: return "windows-1251";
        case 1252: return "windows-1252";
        case 1253: return "windows-1253";
        case 1254: return "windows-1254";
        case 1255: return "windows-1255";
        case 1256: return "windows-1256";
        case 1257: return "windows-1257";
        case 1258: return "windows-1258";

        case 20127: return "US-ASCII";

        case 20866: return "KOI8-R";
        case 20932: return "EUC-JP";
        case 21866: return "KOI8-U";

        case 28591: return "ISO-8859-1";
        case 28592: return "ISO-8859-2";
        case 28593: return "ISO-8859-3";
        case 28594: return "ISO-8859-4";
        case 28595: return "ISO-8859-5";
        case 28596: return "ISO-8859-6";
        case 28597: return "ISO-8859-7";
        case 28598: return "ISO-8859-8";
        case 28599: return "ISO-8859-9";
        case 28603: return "ISO-8859-13";
        case 28605: return "ISO-8859-15";

        case 50220: return "ISO-2022-JP";
        case 50225: return "ISO-2022-KR";

        case 51949: return "EUC-KR";
        case 54936: return "GB18030";

        case 65001: return "UTF-8";

        default:
        {
          std::ostringstream message;
          message << "Unknown codepage " << codepage;
          throw std::invalid_argument(message.str());
        }
      }
    }

    std::string
    windows_encoding_to_utf8(std::string const& string)
    {
      return boost::locale::conv::to_utf<char>(
        string,
        windows_codepage_to_encoding(AreFileApisANSI() ? GetACP() : GetOEMCP()));
    }

    std::string
    utf8_to_windows_encoding(std::string const& string)
    {
      return boost::locale::conv::from_utf<char>(
        string.c_str(),
        windows_codepage_to_encoding(AreFileApisANSI() ? GetACP() : GetOEMCP()));
    }

    std::string
    wstring_to_string(std::wstring const& s)
    {
      int slength = (int)s.length();
      int len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
      std::string r(len, '\0');
      WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);
      return r;
    }
  }
}
