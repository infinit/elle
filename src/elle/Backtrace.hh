#pragma once

#include <array>
#include <string>
#include <vector>

#include <elle/compiler.hh>

#if defined ELLE_WINDOWS or defined ELLE_ANDROID
# define ELLE_HAVE_EXECINFO 0
#elif ! defined __has_include
# define ELLE_HAVE_EXECINFO 1
#elif __has_include(<execinfo.h>)
# define ELLE_HAVE_EXECINFO 1
#else
# define ELLE_HAVE_EXECINFO 0
#endif

#if ELLE_HAVE_EXECINFO
# include <execinfo.h>
#endif

#if defined ELLE_WINDOWS || defined ELLE_ANDROID || !ELLE_HAVE_EXECINFO
# define ELLE_HAVE_BACKTRACE 0
#else
# define ELLE_HAVE_BACKTRACE 1
#endif

namespace elle ELLE_API
{
  /// The description of a function activation.
  ///
  /// Warning: this is serialized in protocol/RPC, the layout can
  /// hardly change without compromising backward compatibility.
  struct StackFrame
  {
    // FIXME: why is this not void*?
#if defined ELLE_WINDOWS
    using Address = uint64_t;
#else
    using Address = int;
#endif

    StackFrame();
    /// Build from a line issued by backtrace_symbols.
    StackFrame(std::string line);

    /// The symbol, demangled if we could.
    std::string symbol;
    std::string symbol_mangled;
    std::string symbol_demangled;
    Address address;
    int offset;
  };

  std::ostream&
  operator<< (std::ostream& output, const StackFrame& frame);


  /// Demangle a C++ symbol.
  ///
  /// @throws std::runtime_error if it fails to demangle.
  std::string
  demangle(const std::string& sym);


  class Backtrace
  {
  public:
    using Frame = StackFrame;

    /// An empty backtrace.
    Backtrace();
    /// A backtrace corresponding to these stack frames.
    Backtrace(std::vector<Frame>);

    struct now_t {};
    static now_t now;

    /// The backtrace leading to the call to this constructor.
    ///
    /// I.e., does not include the call to this ctor.
    ELLE_COMPILER_ATTRIBUTE_ALWAYS_INLINE
    Backtrace(now_t, unsigned skip = 0);

    /// The backtrace leading to the call to this function.
    ///
    /// I.e., does not include the call to this function.
    static inline ELLE_COMPILER_ATTRIBUTE_ALWAYS_INLINE
    Backtrace
    current(unsigned skip = 0);

    void
    strip_base(const Backtrace& base);

    std::vector<Frame> const&
    frames() const;

  private:
    void _resolve();
    std::vector<Frame> _frames;
    bool _resolved = false;
    unsigned _skip = 0;
    static constexpr size_t _callstack_size = 128;
    std::array<void*, _callstack_size> _callstack;
    unsigned _frame_count = 0;
  };

  std::ostream&
  operator<< (std::ostream& output, const Backtrace& bt);
}

#include <elle/Backtrace.hxx>
