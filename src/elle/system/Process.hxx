#ifndef  ELLE_SYSTEM_PROCESS_HXX
# define ELLE_SYSTEM_PROCESS_HXX

namespace elle
{
  namespace system
  {

    Process
    _execute(std::string const& binary)
    {
      return Process{binary};
    }

    Process
    _execute(std::list<std::string>& args,
            std::string const& binary)
    {
      return Process{binary, args};
    }

    template <typename Second, typename... Tail>
    typename std::enable_if<!std::is_convertible<Second, std::string>::value, Process>::type
    _execute(std::list<std::string>& args,
            std::string const& binary,
            Second&& second,
            Tail&&... tail)
    {
      return Process{binary, args, std::forward<Second>(second), std::forward<Tail>(tail)...};
    }

    template <typename Second, typename... Tail>
    typename std::enable_if<std::is_convertible<Second, std::string>::value, Process>::type
    _execute(std::list<std::string>& args,
            std::string const& binary,
            Second&& second,
            Tail&&... tail)
    {
      args.push_back(second);
      return _execute(args, binary, std::forward<Tail>(tail)...);
    }

    template <typename... Args>
    Process _execute(std::string const& binary, Args&&... args)
    {
      std::list<std::string> head_args;
      return _execute(head_args, binary, std::forward<Args>(args)...);
    }

    template <typename... Args>
    Process execute(Args&&... args)
    {
      return _execute(std::forward<Args>(args)...);
    }

    template <typename... Args>
    std::string check_output(Args&&... args)
    {
      Process p{execute(std::forward<Args>(args)...)};
      std::string s;
      while (p.running())
        s += p.read();
      return s;
    }

  }
}

#endif

