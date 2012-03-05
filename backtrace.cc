#ifdef WIN32
# include <nitro/windows.h>
# include <Dbghelp.h>
# include <Tlhelp32.h>
# include <Winbase.h>
# pragma comment(lib, "Dbghelp.lib")
#else
# ifndef ANDROID
#  include <cxxabi.h>
#  include <execinfo.h>
# endif
#endif
#ifndef ANDROID
# include <iomanip>
# include <iostream>
#endif
#include <string>
#include <sstream>

#include <cmath>
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH
#include "backtrace.hh"

namespace nitro
{
#ifndef ANDROID
  static bool
  extract(std::string& str, std::string& chunk, unsigned char until)
  {
    size_t pos = str.find(until);
    if (pos != std::string::npos)
    {
      chunk = str.substr(0, pos);
      str = str.substr(pos + 1);
      return true;
    }
    else
      return false;
  }

  static bool
  discard(std::string& str, unsigned char until)
  {
    std::string ignored;
    return extract(str, ignored, until);
  }
#endif

  std::string
  demangle(const std::string& sym)
  {
#ifdef ANDROID
    return sym;
#else
    std::string err;
# ifdef WIN32
    static const size_t size = 256;
    char output[size];
    static const size_t res = UnDecorateSymbolName(sym.c_str(), output, size, UNDNAME_COMPLETE);
    if (res == 0)
      err = GetLastError();
    else
      return output;
# else
    size_t size;
    int status;
    char* demangled = abi::__cxa_demangle(sym.c_str(), 0, &size, &status);

    switch (status)
    {
      case 0:
      {
        std::string res(demangled);
        free(demangled);
        return res;
      }
      case -1:
        err = "memory allocation failure";
        break;
      case -2:
        err = "not a valid name under the C++ ABI mangling rules";
        break;
      case -3:
        err = "invalid argument";
        break;
      default:
        std::abort();
    }
   throw std::runtime_error("demangling failure");
# endif
#endif
  }

#ifdef WIN32
  class StackWalker
  {
    public:
      StackWalker()
      {
        this->m_hProcess = GetCurrentProcess();

        if (SymInitialize(m_hProcess, NULL, FALSE) == FALSE)
          throw std::runtime_error("SymInitialize");

        DWORD symOptions = SymGetOptions();
        symOptions |= SYMOPT_LOAD_LINES;
        symOptions |= SYMOPT_FAIL_CRITICAL_ERRORS;
        symOptions = SymSetOptions(symOptions);

        // List and load all used modules
        {
          HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
          if (hSnap == (HANDLE) -1)
            throw std::runtime_error("FIXME");

          int cnt = 0;
          bool keepGoing;
          MODULEENTRY32 me;
          me.dwSize = sizeof(me);
          keepGoing = !!Module32First(hSnap, &me);
          while (keepGoing)
          {
            if (SymLoadModule64(m_hProcess, 0, me.szExePath, me.szModule, (DWORD64) me.modBaseAddr, me.modBaseSize) == 0)
              throw std::runtime_error("SymLoadModule64");
            cnt++;
            keepGoing = !!Module32Next(hSnap, &me);
          }
          CloseHandle(hSnap);
          if (cnt <= 0)
            throw std::runtime_error("FIXME");
        }
      }

      virtual ~StackWalker()
      {
        SymCleanup(m_hProcess);
      }

      enum { STACKWALK_MAX_NAMELEN = 1024 }; // max name length for found symbols

      void read(Backtrace& bt)
      {
        HANDLE hThread = GetCurrentThread();
        IMAGEHLP_SYMBOL64 *pSym = NULL;
        IMAGEHLP_MODULE64 Module;
        IMAGEHLP_LINE64 Line;

        CONTEXT c;
        memset(&c, 0, sizeof(CONTEXT));
        // Normally it should be enough to use 'CONTEXT_FULL' (better would be 'CONTEXT_ALL')
        c.ContextFlags = CONTEXT_ALL;
        RtlCaptureContext(&c);

        // init STACKFRAME for first call
        STACKFRAME64 s; // in/out stackframe
        memset(&s, 0, sizeof(s));
        DWORD imageType;
#ifdef _M_IX86
        // normally, call ImageNtHeader() and use machine info from PE header
        imageType = IMAGE_FILE_MACHINE_I386;
        s.AddrPC.Offset = c.Eip;
        s.AddrPC.Mode = AddrModeFlat;
        s.AddrFrame.Offset = c.Ebp;
        s.AddrFrame.Mode = AddrModeFlat;
        s.AddrStack.Offset = c.Esp;
        s.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
        imageType = IMAGE_FILE_MACHINE_AMD64;
        s.AddrPC.Offset = c.Rip;
        s.AddrPC.Mode = AddrModeFlat;
        s.AddrFrame.Offset = c.Rsp;
        s.AddrFrame.Mode = AddrModeFlat;
        s.AddrStack.Offset = c.Rsp;
        s.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
        imageType = IMAGE_FILE_MACHINE_IA64;
        s.AddrPC.Offset = c.StIIP;
        s.AddrPC.Mode = AddrModeFlat;
        s.AddrFrame.Offset = c.IntSp;
        s.AddrFrame.Mode = AddrModeFlat;
        s.AddrBStore.Offset = c.RsBSP;
        s.AddrBStore.Mode = AddrModeFlat;
        s.AddrStack.Offset = c.IntSp;
        s.AddrStack.Mode = AddrModeFlat;
#else
#error "Platform not supported!"
#endif

        pSym = (IMAGEHLP_SYMBOL64 *) new char[sizeof(IMAGEHLP_SYMBOL64) + STACKWALK_MAX_NAMELEN];

        memset(pSym, 0, sizeof(IMAGEHLP_SYMBOL64) + STACKWALK_MAX_NAMELEN);
        pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
        pSym->MaxNameLength = STACKWALK_MAX_NAMELEN;

        memset(&Line, 0, sizeof(Line));
        Line.SizeOfStruct = sizeof(Line);

        memset(&Module, 0, sizeof(Module));
        Module.SizeOfStruct = sizeof(Module);

        while (true)
        {
          // get next stack frame (StackWalk64(), SymFunctionTableAccess64(), SymGetModuleBase64())
          // if this returns ERROR_INVALID_ADDRESS (487) or ERROR_NOACCESS (998), you can
          // assume that either you are done, or that the stack is so hosed that the next
          // deeper frame could not be found.
          // CONTEXT need not to be suplied if imageTyp is IMAGE_FILE_MACHINE_I386!
          if (!StackWalk64(imageType, this->m_hProcess, hThread, &s, &c, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL) )
            break;

          StackFrame frame;
          frame.address = s.AddrPC.Offset;
          frame.offset = 0;

          if (s.AddrPC.Offset == s.AddrReturn.Offset)
            // Endless callstack
            break;
          if (s.AddrPC.Offset != 0)
          {
            // we seem to have a valid PC
            // show procedure info (SymGetSymFromAddr64())
            if (SymGetSymFromAddr64(this->m_hProcess, s.AddrPC.Offset, NULL, pSym) != FALSE)
            {
              frame.symbol = frame.symbol_mangled = frame.symbol_demangled = pSym->Name;
            }
            else
              throw std::runtime_error("SymGetSymFromAddr64");

// 						// Try to get the file/line info
// 						if (SymGetLineFromAddr64(this->m_hProcess, s.AddrPC.Offset, NULL, &Line) != FALSE)
// 						{
// 							entry.lineNumber = Line.LineNumber;
// 							entry.lineFileName = Line.FileName;
// 						}
          }

          bt.push_back(frame);

          if (s.AddrReturn.Offset == 0)
            break;
        }

        delete [] pSym;

      }

      HANDLE m_hProcess;
  };
#endif

  Backtrace::Backtrace()
    : SuperType()
  {
#ifdef WIN32
    // SEGV now :-/
    // static StackWalker walker;
    // walker.read(*this);
#else
# ifndef ANDROID
    static const size_t size = 128;
    void* callstack[size];
    size_t frames = ::backtrace(callstack, size);
    char** strs = backtrace_symbols(callstack, frames);

    for (unsigned i = 0; i < frames; ++i)
    {
      StackFrame frame;
      std::string sym(strs[i]);
      discard(sym, '(');
      if (extract(sym, frame.symbol_mangled, '+'))
      {
        try
        {
          frame.symbol = frame.symbol_demangled = demangle(frame.symbol_mangled);
        }
        catch (const std::runtime_error&)
        {
          frame.symbol = frame.symbol_mangled;
        }

        std::string offset;
        extract(sym, offset, ')');
        {
          std::stringstream stream(offset);
          stream >> std::hex >> frame.offset;
        }
      }
      discard(sym, '[');
      std::string addr;
      extract(sym, addr, ']');


      {
        std::stringstream stream(addr);
        stream >> std::hex >> frame.address;
      }
      this->push_back(frame);
    }
    free(strs);
# endif
#endif
  }

  std::ostream&
  operator<< (std::ostream& out, const StackFrame& frame)
  {
    out << "0x" << std::hex << frame.address;
    if (!frame.symbol.empty())
      out << ": " << frame.symbol << " +0x" << frame.offset;
    else
      out << ": ???";
    return out;
  }

  std::ostream&
  operator<< (std::ostream& out, const Backtrace& bt)
  {
#ifdef ANDROID
    // FIXME: make it clear backtraces are unsupported: print a log in
    // the debugger.
    (void)out;
    (void)bt;
    return out << "<backtrace not supported on Android>";
#else
    unsigned i = 0;
    // Visual expects a float ... don't ask.
    const size_t width = std::log10(float(bt.size())) + 1;
    foreach (const Backtrace::Frame& f, bt)
      out << std::setw(width) << "#" << i++ << " " << f << ": " << std::endl;
    return out;
#endif
  }

  StackFrame::operator std::string() const
  {
    std::stringstream s;
    s << *this;
    return s.str();
  }

}
