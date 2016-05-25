#ifndef ELLE_LOG_SEND_HH
# define ELLE_LOG_SEND_HH

# include <elle/compiler.hh>
# include <elle/log/Logger.hh>
# include <elle/memory.hh>

namespace elle
{
  namespace log
  {
    Logger&
    logger();
    std::unique_ptr<Logger>
    logger(std::unique_ptr<Logger> l); ///< returns previous value

    /// Here the simplest type possible is used (.rodata-located) so
    /// as to make sure that its initialization will always take place
    /// before the other global variables which construction may require
    /// logging.
    namespace detail
    {
      struct Send
      {
      public:
        template <typename... Args>
        Send(elle::log::Logger::Level level,
             elle::log::Logger::Type type,
             bool indent,
             std::string const& component,
             char const* file,
             unsigned int line,
             char const* function,
             char const* fmt,
             Args&&... args);
        ~Send();
        operator bool() const;

      /*------------.
      | Indentation |
      `------------*/
      protected:
        void
        _indent();
        void
        _unindent();
      private:
        bool _proceed;
      public: // used by macros
        static bool _enabled(elle::log::Logger::Type type,
                             elle::log::Logger::Level level,
                             std::string const& component);

      private:
        void _send(elle::log::Logger::Level level,
                   elle::log::Logger::Type type,
                   bool indent,
                   std::string const& component,
                   char const* file,
                   unsigned int line,
                   char const* function,
                   const std::string& msg);
        unsigned int* _indentation;
      };

      template <typename... Args>
      std::string
      print_function_params(Args&&... args);
    }
  }
}

# include <elle/log/Send.hxx>

#endif
