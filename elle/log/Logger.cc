
#include <cassert>

#include <elle/log/Logger.hh>

namespace elle
{
  namespace log
  {

    namespace
    {

      size_t const max_level_idx = static_cast<size_t>(Logger::Level::_max_value);

      std::string const level_strings[max_level_idx] = {
          "TRACE",
          "DEBUG",
          "INFO",
          "WARNING",
          "ERROR",
          "FATAL",
      };

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
      OutStream   streams[max_level_idx];
      Impl(Level level, std::string const& name)
        : level(level)
        , name(name)
        , streams({
                {&std::cerr, false},
                {&std::cerr, false},
                {&std::cerr, false},
                {&std::cerr, false},
                {&std::cerr, false},
                {&std::cerr, false},
            })
      {
      }
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
      Logger::_message(Level level, std::string const& message)
      {
        if (level < this->_impl->level)
          return;
        size_t idx = static_cast<size_t>(level);
        assert(idx < max_level_idx);
        std::ostream* out = this->_impl->streams[idx].out;
        assert(out != nullptr);
        (*out) << '[' << level_strings[idx] << "] " << message << std::endl;
      }

    void
      Logger::stream(Level level, std::ostream& out)
      {
        size_t idx = static_cast<size_t>(level);
        assert(idx < max_level_idx);
        this->_impl->streams[idx] = OutStream{&out, false};
      }

    void
      Logger::stream(Level level, std::unique_ptr<std::ostream>&& out)
      {
        assert(out.get() != nullptr && "Shouldn't set a null out stream");
        size_t idx = static_cast<size_t>(level);
        assert(idx < max_level_idx);
        OutStream old = std::move(this->_impl->streams[idx]);
        this->_impl->streams[idx] = OutStream{out.get(), true};
        out.release();
        if (old.owned)
          delete old.out;
      }

    void
      Logger::stream(std::ostream& out)
      {
        for (size_t idx = 0; idx < max_level_idx; ++idx)
          this->stream(static_cast<Level>(idx), out);
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
