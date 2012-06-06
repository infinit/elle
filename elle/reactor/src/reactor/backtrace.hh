#ifndef REACTOR_BACKTRACE_HH
# define REACTOR_BACKTRACE_HH

# include <vector>

namespace reactor
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
    void strip_base(const Backtrace& base);
    typedef std::vector<StackFrame> SuperType;
    typedef StackFrame Frame;
    static Backtrace current();
  };

  std::ostream& operator<< (std::ostream& output, const StackFrame& frame);
  std::ostream& operator<< (std::ostream& output, const Backtrace& bt);
}

#endif
