#pragma once

#include <vector>
#include <string>

#include <elle/compiler.hh>

namespace elle ELLE_API
{
  struct StackFrame
  {
    std::string symbol;
    std::string symbol_mangled;
    std::string symbol_demangled;
#if defined INFINIT_WINDOWS
    uint64_t address;
#else
    int address;
#endif
    int offset;
    operator std::string() const;
  };

  /// Demangle a C++ symbols.
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
    bool _resolved;
    unsigned _skip;
    static const size_t _callstack_size = 128;
    void* _callstack[_callstack_size];
    unsigned _frame_count;
  };

  std::ostream&
  operator<< (std::ostream& output, const StackFrame& frame);

  std::ostream&
  operator<< (std::ostream& output, const Backtrace& bt);
}

#include <elle/Backtrace.hxx>
