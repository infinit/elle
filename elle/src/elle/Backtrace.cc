#include <cxxabi.h>
#include <iomanip>
#include <iostream>

#include <cstdlib>
#include <string>
#include <sstream>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include <elle/Backtrace.hh>

namespace elle
{
#if !defined(INFINIT_MACOSX)
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

  bool
  demangle(const std::string& sym, std::string& res, std::string& error)
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

  std::string
  demangle(const std::string& sym)
  {
    std::string error;
    std::string res;
    if (!demangle(sym, res, error))
      {
        static boost::format model("%s: demangling failure: %s");
        boost::format fmt(model);
        throw std::runtime_error(str(fmt % sym % error));
      }
    return res;
  }

  Backtrace::Backtrace()
    : SuperType()
  {}

  Backtrace
  Backtrace::_current(void** callstack, size_t frames, unsigned skip)
  {
    Backtrace bt;

    char** strs = backtrace_symbols(callstack, frames);
    for (unsigned i = skip; i < frames; ++i)
    {
      StackFrame frame;
      std::string symbol_mangled;
      std::string addr;
      std::string offset;
      {
        std::string line(strs[i]);
#ifdef INFINIT_MACOSX
        std::string file;
        std::string discard;
        std::stringstream stream(line);
        stream >> discard >> file >> addr >> symbol_mangled >> discard >> offset;
#else
        discard(line, '(');
        if (extract(line, symbol_mangled, '+'))
          extract(line, offset, ')');
        discard(line, '[');
        extract(line, addr, ']');
#endif
      }
      frame.symbol_mangled = symbol_mangled;
      if (!symbol_mangled.empty())
      {
        std::string error;
        if (!demangle(symbol_mangled, frame.symbol, error))
          frame.symbol = symbol_mangled;
      }
      {
        std::stringstream stream(offset);
        stream >> std::hex >> frame.offset;
      }
      {
        std::stringstream stream(addr);
#ifdef INFINIT_MACOSX
        stream >> frame.address;
#else
        stream >> std::hex >> frame.address;
#endif
      }
      bt.push_back(frame);
    }
    free(strs);

    return bt;
  }

  void
  Backtrace::strip_base(const Backtrace& base)
  {
    auto other = base.rbegin();

    while (!this->empty() && other != base.rend()
           && this->back().address == other->address)
      {
        this->pop_back();
        ++other;
      }
  }

  std::ostream&
  operator<< (std::ostream& out, const StackFrame& frame)
  {
    out << (boost::format("0x%0" + boost::lexical_cast<std::string>(2 * sizeof(void*)) + "x: ") % frame.address);

    if (!frame.symbol.empty())
      out << (boost::format("%s +0x%x") % frame.symbol % frame.offset);
    else
      out << "???";
    return out;
  }

  std::ostream&
  operator<< (std::ostream& out, const Backtrace& bt)
  {
    unsigned i = 0;
    // Visual expects a float ... don't ask.
    const size_t width = std::log10(float(bt.size())) + 1;
    BOOST_FOREACH (const Backtrace::Frame& f, bt)
      {
        boost::format fmt("#%-" + boost::lexical_cast<std::string>(width) + "d %s\n");
        out << (fmt % i++ % f);
      }
    return out;
  }

  StackFrame::operator std::string() const
  {
    std::stringstream s;
    s << *this;
    return s.str();
  }
}
