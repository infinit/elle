#ifndef ELLE_LOG_SEND_HH
# define ELLE_LOG_SEND_HH

# include <elle/compiler.hh>
# include <elle/log/Logger.hh>

namespace elle
{
  namespace log
  {
    void
    logger(std::unique_ptr<Logger> logger);

    /// Here the simplest type possible is used (.rodata-located) so
    /// as to make sure that its initialization will always take place
    /// before the other global variables which construction may require
    /// logging.
    namespace detail
    {
      struct Send
      {
      public:
        template <typename ... Args>
        Send(elle::log::Logger::Level level,
                     elle::log::Logger::Type type,
                     elle::String const& component,
                     char const* file,
                     unsigned int line,
                     char const* function,
                     char const* fmt,
                     Args const& ... args);
        ~Send();
        operator bool() const;
      private:
        bool _proceed;
        static bool _enabled(elle::log::Logger::Type type,
                             elle::log::Logger::Level level,
                             elle::String const& component);

      private:
        void _send(elle::log::Logger::Level level,
                   elle::log::Logger::Type type,
                   std::string const& component,
                   char const* file,
                   unsigned int line,
                   char const* function,
                   const std::string& msg);
        void _send(elle::log::Logger::Level level,
                   elle::log::Logger::Type type,
                   std::string const& component,
                   std::string const& msg);
      };

      template <typename... Args>
      std::string
      print_function_params(Args const&... args);
    }
  }
}

# include <elle/log/Send.hxx>

#endif
