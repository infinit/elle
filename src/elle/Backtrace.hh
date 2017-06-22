#pragma once

#include <array>
#include <string>
#include <vector>

#include <elle/compiler.hh>

namespace elle ELLE_API
{
  /// The description of a function activation.
  ///
  /// Warning: this is serialized in protocol/RPC, the layout can
  /// hardly change without compromising backward compatibility.
  struct StackFrame
  {
    // FIXME: why is this not void*?
#if defined INFINIT_WINDOWS
    using Address = uint64_t;
#else
    using Address = int;
#endif

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
    Backtrace();
    Backtrace(std::vector<StackFrame> const&);
    using Frame = StackFrame;

    /// The backtrace leading to the call to this function.
    ///
    /// I.e., does not include this function.
    static inline ELLE_COMPILER_ATTRIBUTE_ALWAYS_INLINE
    Backtrace
    current(unsigned skip = 0);

    void
    strip_base(const Backtrace& base);

    std::vector<StackFrame> const&
    frames() const;

  private:
    void _resolve();
    std::vector<StackFrame> _frames;
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
