#ifndef  ELLE_SYSTEM_PROCESS_HXX
# define ELLE_SYSTEM_PROCESS_HXX

#include <elle/assert.hh>

namespace elle
{
  namespace system
  {

    namespace detail
    {

      // Dispatch arguments and merge configurations.
      struct Command
        : public ProcessCommandConcept<Command>
      {
      protected:
        std::unique_ptr<Command>  _next;
        std::unique_ptr<Process>  _process;
        ProcessConfig             _config;
        std::string               _binary;
        std::list<std::string>    _args;

      public:
        template <typename... Args>
        explicit
        Command(Args&&... args)
          : _next{nullptr}
        {
          this->apply(std::forward<Args>(args)...);
          ELLE_ASSERT(!this->_binary.empty());
        }

        Command(Command&& other)
          : _next{std::move(other._next)}
          , _process{std::move(other._process)}
          , _config{std::move(other._config)}
          , _binary{std::move(other._binary)}
          , _args{std::move(other._args)}
        {
          ELLE_ASSERT(!this->_binary.empty());
        }

        Command&
        operator =(Command&& other)
        {
          if (this != &other)
            {
              this->_next = std::move(other._next);
              this->_process = std::move(other._process);
              this->_config = std::move(other._config);
              this->_binary = std::move(other._binary);
              this->_args = std::move(other._args);

              ELLE_ASSERT(!this->_binary.empty());
            }
          return *this;
        }

        Command& connect_stdout(Command&& other)
        {
          auto& last = this->last();
          last._config.connect_stdout(other._config);
          last._next.reset(new Command{std::move(other)});
          return *this;
        }

        Command&& operator |(Command&& other)
        {
          this->connect_stdout(std::move(other));
          return std::move(*this);
        }

        Command& execute();
        Command& wait();
        Process::StatusCode wait_status();

        Command& last()
        {
          Command* ptr = this;
          while (ptr->_next != nullptr)
            ptr = ptr->_next.get();
          return *ptr;
        }

        Process& process();

      protected:
        void _append(Command&& other)
        {
          this->last()._next.reset(new Command{std::move(other)});
        }


      protected:
        // helpers
# define _ELLE_SYSTEM_DISPATCH_APPLY(__first_arg)                             \
        template <typename... Args>                                           \
        inline                                                                \
        void apply(__first_arg, Args&&... args__)                             \
/**/
# define _ELLE_SYSTEM_DISPATCH_APPLY_ARGS()                                   \
          this->apply(std::forward<Args>(args__)...)                          \
/**/

        // binary or arg
        _ELLE_SYSTEM_DISPATCH_APPLY(std::string const& str)
        {
          if (this->_binary.empty())
            this->_binary = str;
          else
            this->_args.push_back(str);
          _ELLE_SYSTEM_DISPATCH_APPLY_ARGS();
        }

        // args
        _ELLE_SYSTEM_DISPATCH_APPLY(std::list<std::string> const& args)
        {
          for (auto const& arg: this->_args)
            this->_args.push_back(arg);
          _ELLE_SYSTEM_DISPATCH_APPLY_ARGS();
        }

        // config
        _ELLE_SYSTEM_DISPATCH_APPLY(ProcessConfig&& config)
        {
          this->_config.update(std::move(config));
          _ELLE_SYSTEM_DISPATCH_APPLY_ARGS();
        }

        // config model
        _ELLE_SYSTEM_DISPATCH_APPLY(ProcessConfigModel const model)
        {
          this->_config.update(process_config(model));
          _ELLE_SYSTEM_DISPATCH_APPLY_ARGS();
        }

        // A command
        _ELLE_SYSTEM_DISPATCH_APPLY(Command&& command)
        {
          if (this->_binary.empty())
              *this = std::move(command);
          else
              this->_next.reset(new Command{std::move(command)});
          this->last().apply(std::forward<Args>(args__)...);
        }

        // end of recursion
        void apply() {}
      };

      template<typename... Args>
      struct contains_convertible_to;

      template<typename To, typename First, typename... Args>
      struct contains_convertible_to<To, First, Args...>
      {
        static bool const value =
              std::is_convertible<First, To>::value
          or  contains_convertible_to<To, Args...>::value
        ;
      };

      template<typename To>
      struct contains_convertible_to<To>
      {
        static bool const value = false;
      };

    }

    template<typename... Args>
    inline
    detail::Command command(Args&&... args)
    {
      static_assert(
            detail::contains_convertible_to<std::string, Args...>::value
        ||  detail::contains_convertible_to<detail::Command, Args...>::value,
        "execute() arguments should contains at least a binary name or a command"
      );
      return detail::Command(std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline
    Process::StatusCode execute(Args&&... args)
    {
      detail::Command cmd{command(std::forward<Args>(args)...)};
      cmd.execute();
      return cmd.wait_status();
    }


    template <typename... Args>
    std::string check_output(Args&&... args)
    {
      detail::Command cmd{command(std::forward<Args>(args)...), check_output_config};
      cmd.execute();
      std::string s;
      while (cmd.last().process().running())
        s += cmd.last().process().read();
      cmd.wait();
      return s;
    }

  }
}

#endif

