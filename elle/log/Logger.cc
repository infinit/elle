#include <cassert>

#include <elle/log/Logger.hh>

namespace elle
{
  namespace log
  {
    void
    Logger::message(Level level,
                    elle::log::Logger::Type type,
                    std::string const& msg)
    {
      this->_message(level, type, msg);
    }

#define ELLE_LOG_LEVEL_MESSAGE(Lvl)                     \
    void                                                \
    Logger::Lvl(std::string const& msg)                 \
    {                                                   \
    return this->message(Logger::Level::Lvl,            \
                         Logger::Type::info,            \
                         msg);                          \
    }                                                   \

    ELLE_LOG_LEVEL_MESSAGE(log);
    ELLE_LOG_LEVEL_MESSAGE(trace);
    ELLE_LOG_LEVEL_MESSAGE(debug);
    ELLE_LOG_LEVEL_MESSAGE(dump);
#undef ELLE_LOG_LEVEL_MESSAGE

    namespace
    {
      struct OutStream
      {
        std::ostream* out;
        bool          owned;
        OutStream(std::ostream* out, bool owned) : out(out), owned(owned) {}
        OutStream(OutStream&& other) : out(other.out), owned(other.owned)
        {
          other.out = nullptr;
          other.owned = false;
        }
        OutStream& operator =(OutStream&& other)
        {
          if (this != &other)
            {
              if (this->owned)
                {
                  delete this->out;
                  this->out = nullptr;
                }
              this->out = other.out;
              this->owned = other.owned;
              other.out = nullptr;
              other.owned = false;
            }
          return *this;
        }
        ~OutStream()
        {
          if (this->owned)
            {
              delete this->out;
              this->out = nullptr;
            }
        }
      };

    } // !anonymous

    struct Logger::Impl
    {
      Level       level;
      std::string name;
      Impl(Level level, std::string const& name)
        : level(level)
        , name(name)
      {}
    };

    Logger::Logger(Logger::Level level, std::string const& name)
      : _impl(nullptr)
    {
      this->_impl = new Impl{
        level,
        name,
      };
    }

    Logger::~Logger()
    {
      delete this->_impl;
      this->_impl = nullptr;
    }

    void
    Logger::_message(Level level,
                     elle::log::Logger::Type type,
                     std::string const& message)
    {
      if (level <= _impl->level)
        std::cerr << message << std::endl;
    }

    std::string const&
    Logger::name() const
    {
      return this->_impl->name;
    }

    void
    Logger::name(std::string const& name_)
    {
      this->_impl->name = name_;
    }

    Logger::Level
    Logger::level() const
    {
      return this->_impl->level;
    }

    void
    Logger::level(Level level_)
    {
      this->_impl->level = level_;
    }

  }
}
