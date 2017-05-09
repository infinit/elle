#ifdef INFINIT_WINDOWS
# include <windows.h>
# include <dbghelp.h>
#endif
#include <cxxabi.h>
#include <cmath>

#include <cstdlib>
#include <iomanip>
#include <string>
#include <sstream>

#include <elle/Backtrace.hh>
#include <elle/err.hh>
#include <elle/printf.hh>
#include <elle/utils.hh>

namespace elle
{
  namespace
  {
#if !defined INFINIT_MACOSX && !defined INFINIT_WINDOWS && !defined INFINIT_ANDROID
    bool
    extract(std::string& str, std::string& chunk, unsigned char until)
    {
      size_t pos = str.find(until);
      if (pos == std::string::npos)
        return false;
      else
      {
        chunk = str.substr(0, pos);
        str = str.substr(pos + 1);
        return true;
      }
    }

    bool
    discard(std::string& str, unsigned char until)
    {
      std::string ignored;
      return extract(str, ignored, until);
    }
#endif

    bool
    demangle_impl(const std::string& sym, std::string& res, std::string& error)
    {
      size_t size;
      int status;
      char* demangled = abi::__cxa_demangle(sym.c_str(), 0, &size, &status);

      switch (status)
      {
        case 0:
        {
          res = demangled;
          free(demangled);
          return true;
        }
        case -1:
          error = "memory allocation failure";
          return false;
        case -2:
          error = "not a valid name under the C++ ABI mangling rules";
          return false;
        case -3:
          error = "invalid argument";
          return false;
        default:
          std::abort();
      }
    }
  }

  std::string
  demangle(const std::string& sym)
  {
    std::string error;
    std::string res;
    if (demangle_impl(sym, res, error))
      return res;
    else
      elle::err<std::runtime_error>("%s: demangling failure: %s",
                                    sym, res);
  }

  Backtrace::Backtrace()
    : _resolved(false)
    , _skip(0)
    , _frame_count(0)
  {}

  Backtrace::Backtrace(std::vector<StackFrame> const& sf)
  : _frames(sf)
  , _resolved(true)
  , _skip(0)
  , _frame_count(0)
  {}

  void
  Backtrace::_resolve()
  {
    if (this->_resolved)
      return;
#if defined INFINIT_WINDOWS
       /*
      auto initialize = []
        {
          HANDLE process = GetCurrentProcess();
          if (!::SymInitialize(process, NULL, TRUE))
            throw std::runtime_error("unable to initialize debug symbols");
          return process;
        };
      static auto process = initialize();
      void* stack[128];
      long unsigned int hash = 0;
      int frames = CaptureStackBackTrace(0, sizeof(stack), stack, &hash);
      ::SYMBOL_INFO* symbol;
      symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
      symbol->MaxNameLen = 255;
      symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

      for (int i = 0; i < frames; i++)
      {
        ::SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
        res.push_back(StackFrame{symbol->Name, symbol->Name, symbol->Name,
              symbol->Address, 0});
      }
      ::free(symbol);
    */
#elif !defined INFINIT_ANDROID && !defined NO_EXECINFO
    char** strs = backtrace_symbols(this->_callstack, this->_frame_count);
    for (unsigned i = this->_skip; i < this->_frame_count; ++i)
    {
      StackFrame frame;
      std::string symbol_mangled;
      std::string addr;
      std::string offset;
      {
        std::string line(strs[i]);
# ifdef INFINIT_MACOSX
        std::string file;
        std::string discard;
        std::stringstream stream(line);
        stream >> discard >> file >> addr >> symbol_mangled >> discard >> offset;
# else
        discard(line, '(');
        if (extract(line, symbol_mangled, '+'))
          extract(line, offset, ')');
        discard(line, '[');
        extract(line, addr, ']');
# endif
      }
      frame.symbol_mangled = symbol_mangled;
      if (!symbol_mangled.empty())
      {
        std::string error;
        if (!demangle_impl(symbol_mangled, frame.symbol, error))
          frame.symbol = symbol_mangled;
      }
      {
        std::stringstream stream(offset);
        stream >> std::hex >> frame.offset;
      }
      {
        std::stringstream stream(addr);
# ifdef INFINIT_MACOSX
        stream >> frame.address;
# else
        stream >> std::hex >> frame.address;
# endif
      }
      this->_frames.push_back(frame);
    }
    free(strs);
#endif
    this->_resolved = true;
  }

  const std::vector<StackFrame>&
  Backtrace::frames() const
  {
    elle::unconst(this)->_resolve();
    return this->_frames;
  }

  void
  Backtrace::strip_base(const Backtrace& base)
  {
    this->_resolve();
    elle::unconst(base)._resolve();
    auto other = base.frames().rbegin();
    auto& f = this->_frames;

    // The function address could change as observed in CentOS.
    while (!f.empty() && other != base.frames().rend()
           && (f.back().address == other->address
               || f.back().symbol == other->symbol))
    {
      f.pop_back();
      ++other;
    }
  }

  std::ostream&
  operator<< (std::ostream& out, const StackFrame& frame)
  {
    // Two hex digits per byte.
    auto width = std::to_string(2 * sizeof(void*));
    elle::fprintf(out, "0x%0" + width + "x: ", frame.address);
    if (frame.symbol.empty())
      out << "???";
    else
      elle::fprintf(out, "%s +0x%x", frame.symbol, frame.offset);
    return out;
  }

  std::ostream&
  operator<< (std::ostream& out, const Backtrace& bt)
  {
    unsigned i = 0;
    // Visual expects a float ... don't ask.
    const size_t width = std::log10(float(bt.frames().size())) + 1;
    for (const auto& f: bt.frames())
      fprintf(out, "#%-" + std::to_string(width) + "d %s\n",
              i++, f);
    return out;
  }

  StackFrame::operator std::string() const
  {
    std::stringstream s;
    s << *this;
    return s.str();
  }
}
