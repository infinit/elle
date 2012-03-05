#ifndef NITRO_BACKTRACE_HH
# define NITRO_BACKTRACE_HH

# include <vector>

namespace nitro
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
	};

	std::ostream& operator<< (std::ostream& output, const StackFrame& frame);
	std::ostream& operator<< (std::ostream& output, const Backtrace& bt);
}

#endif
