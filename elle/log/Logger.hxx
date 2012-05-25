#ifndef  ELLE_LOG_LOGGER_HXX
# define ELLE_LOG_LOGGER_HXX

# include <elle/print.hh>

namespace elle
{
  namespace log
  {

    template<typename... T>
    void
      Logger::message(Level level, T const&... values)
      {
        this->_message(level, elle::stringify(values...));
      }

    template<typename... T>
    void
      Logger::trace(T const&... values)
      {
        return this->message(Logger::Level::trace, values...);
      }

    template<typename... T>
    void
      Logger::debug(T const&... values)
      {
        return this->message(Logger::Level::debug, values...);
      }

    template<typename... T>
    void
      Logger::info(T const&... values)
      {
        return this->message(Logger::Level::info, values...);
      }

    template<typename... T>
    void
      Logger::warn(T const&... values)
      {
        return this->message(Logger::Level::warn, values...);
      }

    template<typename... T>
    void
      Logger::error(T const&... values)
      {
        return this->message(Logger::Level::error, values...);
      }

    template<typename... T>
    void
      Logger::fatal(T const&... values)
      {
        return this->message(Logger::Level::fatal, values...);
      }

  }
}

#endif
