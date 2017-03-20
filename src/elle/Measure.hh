#pragma once

#ifdef ELLE_WITH_MEASURE
# include <atomic>
# include <chrono>
# include <iostream>
# include <string>

# include <elle/memory.hh>
# include <elle/printf.hh>
#endif

#define ELLE_MEASURE_INSTANCE(_name)                                    \
  ::elle::Measure(__FILE__, __LINE__, _name)                            \
  /**/

#define ELLE_MEASURE_SCOPE_INTERNAL(_name)                                   \
  auto BOOST_PP_CAT(_elle_mesure_, __LINE__) = ELLE_MEASURE_INSTANCE(_name)  \
/**/

#define ELLE_MEASURE_SCOPE(_name)                                       \
  ELLE_MEASURE_SCOPE_INTERNAL(_name);                                   \
  (void)BOOST_PP_CAT(_elle_mesure_, __LINE__)                           \
  /**/

#define ELLE_MEASURE(_name)                                             \
  if (ELLE_MEASURE_SCOPE_INTERNAL(_name))                               \
    (void)BOOST_PP_CAT(_elle_mesure_, __LINE__);                        \
  else                                                                  \
    /**/

namespace elle
{
  /// A class to measure execution time of a given scope.
  ///
  /// \code{.cc}
  ///
  /// void
  /// foo()
  /// {
  ///   ELLE_MEASURE("global");
  ///   for (auto i: data)
  ///   {
  ///     ELLE_MEASURE("first loop");
  ///     {
  ///       for (auto j: i.data())
  ///       {
  ///         ELLE_MEASURE("second loop");
  ///         process(j);
  ///       }
  ///     }
  ///   }
  /// }
  ///
  ///
  /// \endcode
  struct Measure
  {
#ifdef ELLE_WITH_MEASURE
    Measure(char const* file, unsigned int line, std::string name)
      : file{file}
      , line{line}
      , name{name}
    {
      _indent()++;
    }

    void
    end()
    {
      if (!this->done)
      {
        this->done = true;
        using namespace std::chrono;
        auto d = duration_cast<milliseconds>(system_clock::now() - this->start);
        elle::fprintf(std::cout,
                      "%s %s took %s ms (%s:%s)\n",
                      std::string(_indent(), '#'),
                      this->name,
                      d.count(),
                      this->file,
                      this->line);
        _indent()--;
      }
    }

    ~Measure()
    {
      this->end();
    }

    char const* file;
    unsigned int line;
    std::string const name;
    std::chrono::system_clock::time_point const start
      = std::chrono::system_clock::now();
    bool done = false;

  private:
    static
    std::atomic_size_t& _indent()
    {
      static std::atomic_size_t i;
      return i;
    }
#else
    // Empty implementation
    template <typename... Args>
    inline Measure(Args&&...) {}
    inline void end() {}
#endif
  public:
     operator bool() const { return false; }
  };
}
