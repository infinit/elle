#include <elle/Backtrace.hh>

#ifdef ELLE_WINDOWS
# include <elle/windows.hh>
# include <dbghelp.h>
#endif
#include <cxxabi.h>
#include <cmath>

#include <cstdlib>
#include <iomanip>
#include <string>
#include <sstream>

#include <elle/err.hh>
#include <elle/printf.hh>
#include <elle/utils.hh>

ELLE_LOG_COMPONENT("elle.Backtrace");

#if ! ELLE_HAVE_EXECINFO
# warning "Backtrace support disabled"
#endif

namespace elle
{
  namespace
  {
#if !defined ELLE_MACOS
    /// Whether `until` is in `str`.
    /// If it is, set `str` to the part before, and `chunk` to the part after.
    bool
    extract(std::string& str, std::string& chunk, char until)
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
    discard(std::string& str, char until)
    {
      auto _ = std::string{};
      return extract(str, _, until);
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
        res = demangled;
        free(demangled);
        return true;
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

    bool
    demangle_impl(const std::string& sym, std::string& res)
    {
      auto _ = std::string{};
      return demangle_impl(sym, res, _);
    }

    template <typename T = int>
    T
    from_hex(std::string const& str)
    {
      auto res = T{};
      auto&& s = std::stringstream(str);
      s >> std::hex >> res;
      return res;
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
                                    sym, error);
  }

  /*-------------.
  | StackFrame.  |
  `-------------*/

  StackFrame::StackFrame() = default;

  StackFrame::StackFrame(std::string line)
  {
    std::string addr;
    std::string offset;
    {
      ELLE_DUMP("line: %s", line);
# ifdef ELLE_MACOS
      std::string file;
      std::string _;
      auto&& s = std::stringstream(line);
      s >> _ >> file >> addr >> this->symbol_mangled >> _ >> offset;
# else
      discard(line, '(');
      if (extract(line, this->symbol_mangled, '+'))
        extract(line, offset, ')');
      discard(line, '[');
      extract(line, addr, ']');
# endif
    }
    demangle_impl(this->symbol_mangled, this->symbol_demangled);
    this->symbol
      = this->symbol_demangled.empty() ? this->symbol_mangled
      : this->symbol_demangled;
    {
      auto&& s = std::stringstream(addr);
# ifdef ELLE_MACOS
      s >> this->address;
# else
      s >> std::hex >> this->address;
# endif
    }
    this->offset = from_hex(offset);
  }

  std::ostream&
  operator<< (std::ostream& out, const StackFrame& frame)
  {
    // Two hex digits per byte.
    auto const fmt = "0x%0" + std::to_string(2 * sizeof(void*)) + "x: ";
    elle::fprintf(out, fmt, frame.address);
    if (frame.symbol.empty())
      out << "???";
    else
      elle::fprintf(out, "%s +0x%x", frame.symbol, frame.offset);
    return out;
  }

  /*------------.
  | Backtrace.  |
  `------------*/

  Backtrace::now_t Backtrace::now;

  Backtrace::Backtrace() = default;

  Backtrace::Backtrace(std::vector<Frame> sf)
    : _frames(std::move(sf))
    , _resolved(true)
  {}

  void
  Backtrace::_resolve()
  {
    ELLE_DEBUG("resolve {} frames", this->_frame_count);
    if (this->_resolved)
      return;
#if ELLE_HAVE_BACKTRACE
    char** strs = backtrace_symbols(this->_callstack.data(), this->_frame_count);
    for (unsigned i = this->_skip; i < this->_frame_count; ++i)
      this->_frames.emplace_back(strs[i]);
    free(strs);
#endif
    this->_resolved = true;
  }

  auto
  Backtrace::frames() const
    -> const std::vector<Frame>&
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
  operator<< (std::ostream& out, const Backtrace& bt)
  {
    auto const& fs = bt.frames();
    unsigned i = 0;
    size_t const width = (fs.empty() ? 0 : std::log10(fs.size())) + 1;
    auto const fmt = "#%-" + std::to_string(width) + "d %s\n";
    for (const auto& f: fs)
      fprintf(out, fmt, i++, f);
    return out;
  }
}
