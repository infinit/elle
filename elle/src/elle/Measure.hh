#ifndef ELLE_MEASURE_HH
# define ELLE_MEASURE_HH

# ifdef ELLE_WITH_MEASURE
#  include <elle/memory.hh>
#  include <elle/printf.hh>

#  include <atomic>
#  include <chrono>
#  include <iostream>
#  include <string>
# endif

# define ELLE_MEASURE_INSTANCE(_name)                                         \
  ::elle::Measure(__FILE__, __LINE__, _name)                                  \
/**/

# define ELLE_MEASURE_SCOPE(_name)                                            \
  auto BOOST_PP_CAT(_elle_mesure_, __LINE__) =                                \
    ::elle::Measure(__FILE__, __LINE__, _name)                                \
/**/

# define ELLE_MEASURE(_name)                                                  \
  if (ELLE_MEASURE_SCOPE(_name))                                              \
    ;                                                                         \
  else                                                                        \
/**/

namespace elle
{
  struct Measure
  {
# ifdef ELLE_WITH_MEASURE
    std::chrono::system_clock::time_point const start;
    char const* file;
    unsigned int line;
    std::string const name;
    bool done;

    Measure(char const* file,
            unsigned int line,
            std::string name):
      start{std::chrono::system_clock::now()},
      file{file},
      line{line},
      name{name},
      done{false}
    { _indent()++; }

    void
    end()
    {
      if (this->done)
        return;
      this->done = true;
      using namespace std::chrono;
      auto d = duration_cast<milliseconds>(system_clock::now() - this->start);
      elle::printf("%s %s took %s ms (%s:%s)\n",
                   std::string(_indent(), '#'),
                   this->name,
                   d.count(),
                   this->file,
                   this->line);
      _indent()--;
    }

    ~Measure()
    {
      this->end();
    }


  private:
    static
    std::atomic_size_t& _indent()
    {
      static std::atomic_size_t i;
      return i;
    }
# else
    // Empty implementation
    template <typename... Args>
    inline Measure(Args&&...) {}
    inline void end() {}
# endif
     operator bool() const { return false; }
  };
}

#endif
