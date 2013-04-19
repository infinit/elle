#ifndef ELLE_BACKTRACE_HH
# define ELLE_BACKTRACE_HH

# include <vector>
# include <string>

# include <elle/compiler.hh>

namespace elle
{
  struct StackFrame
  {
    std::string symbol;
    std::string symbol_mangled;
    std::string symbol_demangled;
    int address;
    int offset;
    operator std::string() const;
  };

  std::string	demangle(const std::string& sym);

  class Backtrace: public std::vector<StackFrame>
  {
  public:
    Backtrace();
    typedef std::vector<StackFrame> SuperType;
    typedef StackFrame Frame;
    static inline ELLE_COMPILER_ATTRIBUTE_ALWAYS_INLINE
    Backtrace
    current();
    void
    strip_base(const Backtrace& base);
  private:
    static
    Backtrace
    _current(void** callstack, size_t frames);
  };

  std::ostream& operator<< (std::ostream& output, const StackFrame& frame);
  std::ostream& operator<< (std::ostream& output, const Backtrace& bt);
}

# include <elle/Backtrace.hxx>

#endif
