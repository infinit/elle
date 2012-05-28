#include <cxxabi.h>
#include <execinfo.h>
#include <iomanip>
#include <iostream>

#include <cstdlib>
#include <string>
#include <sstream>

#include <elle/backtrace.hh>

#include <boost/format.hpp>

namespace elle
{
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

  std::string
  demangle(const std::string& sym)
  {
    std::string err;
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
    static boost::format fmt("%s: demangling failure: %s");
    throw std::runtime_error(str(fmt % sym % err));
  }

  Backtrace::Backtrace()
    : SuperType()
  {
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
    unsigned i = 0;
    // Visual expects a float ... don't ask.
    const size_t width = std::log10(float(bt.size())) + 1;
    foreach (const Backtrace::Frame& f, bt)
      out << std::setw(width) << "#" << i++ << " " << f << ": " << std::endl;
    return out;
  }

  StackFrame::operator std::string() const
  {
    std::stringstream s;
    s << *this;
    return s.str();
  }

}
