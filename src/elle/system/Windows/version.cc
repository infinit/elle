#include <windows.h>
#include <string>
#include <sstream>

#include <elle/system/Windows/version.hh>

using PGNSI = void (WINAPI *)(LPSYSTEM_INFO);
using PGPI = BOOL (WINAPI *)(DWORD, DWORD, DWORD, DWORD, PDWORD);

#define VER_SUITE_WH_SERVER	0x00008000

namespace elle
{
  namespace system
  {
    namespace windows
    {
      // Return a formated string of the version.
      // e.g.: 7 (Professional Edition) (build 9200) (x64)
      std::string
      version_name()
      {
        SYSTEM_INFO si;
        ZeroMemory(&si, sizeof(SYSTEM_INFO));
        BOOL bOsVersionInfoEx;
        DWORD dwType;
        OSVERSIONINFOEX osvi;
        ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
        bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osvi);
        if (bOsVersionInfoEx == 0)
          return "";
        PGNSI pGNSI = (PGNSI) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
        if (pGNSI != NULL)
          pGNSI(&si);
        else
          GetSystemInfo(&si); // Check for unsupported OS
        if (VER_PLATFORM_WIN32_NT != osvi.dwPlatformId || osvi.dwMajorVersion <= 4)
          return "too old (before XP)";

        std::stringstream os;
        os << "(" << osvi.dwMajorVersion << "." << osvi.dwMinorVersion << ") ";
        {
          // Name.
          if (osvi.dwMajorVersion == 6)
          {
            if (osvi.dwMinorVersion == 0)
            {
              if (osvi.wProductType == VER_NT_WORKSTATION)
                os << "Windows Vista";
              else
                os << "Windows Server 2008";
            }
            else if (osvi.dwMinorVersion == 1)
            {
              if (osvi.wProductType == VER_NT_WORKSTATION)
                os << "Windows 7";
              else
                os << "Windows Home Server / Server 2008 R2";
            }
            else if (osvi.dwMinorVersion == 2)
            {
              if (osvi.wProductType == VER_NT_WORKSTATION)
                os << "Windows 8";
              else
                os << "Windows Server 2012";
            }
            else if (osvi.dwMinorVersion == 3)
            {
              if (osvi.wProductType == VER_NT_WORKSTATION)
                os << "Windows 8.1";
              else
                os << "Windows Server 2012 R2";
            }
          }
          else if(osvi.dwMajorVersion == 5)
          {
            if (osvi.dwMinorVersion == 0)
            {
              os << "Windows 2000";
            }
            else if (osvi.dwMinorVersion == 1)
            {
              os << "Windows XP ";
            }
            else if (osvi.dwMinorVersion == 2)
            {
              if (osvi.wProductType == VER_NT_WORKSTATION &&
                  si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
              {
                os <<  "Windows XP Professional x64 Edition";
              }
              else if (osvi.wProductType == VER_NT_WORKSTATION)
                os << "Windows XP";
              else
                os << "Windows Home Server / Server 2003";
            }
          }
          // Edition (if applicable).
          PGPI pGPI = (PGPI) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetProductInfo");
          pGPI(osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType);
          std::stringstream edition;
          switch (dwType)
          {
            case PRODUCT_ULTIMATE:
              edition << "Ultimate";
              break;
            case PRODUCT_PROFESSIONAL:
              edition << "Professional";
              break;
            case PRODUCT_HOME_PREMIUM:
              edition << "Home Premium";
              break;
            case PRODUCT_HOME_BASIC:
              edition << "Home Basic";
              break;
            case PRODUCT_ENTERPRISE:
              edition << "Enterprise";
              break;
            case PRODUCT_BUSINESS:
              edition << "Business";
              break;
            case PRODUCT_STARTER:
              edition << "Starter";
              break;
            case PRODUCT_CLUSTER_SERVER:
              edition << "Cluster Server";
              break;
            case PRODUCT_DATACENTER_SERVER:
              edition << "Datacenter";
              break;
            case PRODUCT_DATACENTER_SERVER_CORE:
              edition << "Datacenter (core installation)";
              break;
            case PRODUCT_ENTERPRISE_SERVER:
              edition << "Enterprise";
              break;
            case PRODUCT_ENTERPRISE_SERVER_CORE:
              edition << "Enterprise (core installation)";
              break;
            case PRODUCT_ENTERPRISE_SERVER_IA64:
              edition << "Enterprise for Itanium-based Systems";
              break;
            case PRODUCT_SMALLBUSINESS_SERVER:
              edition << "Small Business Server";
              break;
            case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
              edition << "Small Business Server Premium";
              break;
            case PRODUCT_STANDARD_SERVER:
              edition << "Standard";
              break;
            case PRODUCT_STANDARD_SERVER_CORE:
              edition << "Standard (core installation)";
              break;
            case PRODUCT_WEB_SERVER:
              edition << "Web Server";
              break;
          }
          if (!edition.str().empty())
            os << "(" << edition.str() << " Edition)";

          // Build.
          if(strlen(osvi.szCSDVersion) > 0)
          {
            os << " " << osvi.szCSDVersion;
          }
          os << " (build " << osvi.dwBuildNumber << ")";
          if (osvi.dwMajorVersion >= 6)
          {
            os << " (";
            switch (si.wProcessorArchitecture)
            {
              case PROCESSOR_ARCHITECTURE_AMD64:
                os << "x64";
                break;
              case PROCESSOR_ARCHITECTURE_IA64:
                os << "ia64";
                break;
              case PROCESSOR_ARCHITECTURE_ARM:
                os << "arm";
                break;
              case PROCESSOR_ARCHITECTURE_INTEL:
                os << "x86";
                break;
              case PROCESSOR_ARCHITECTURE_UNKNOWN:
              default:
                os << "uknown";
            }
            os << ")";
          }
        }
        return os.str();
      }
    }
  }
}
