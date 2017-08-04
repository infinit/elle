#pragma once

#include <elle/Duration.hh>
#include <elle/attribute.hh>
#include <elle/compiler.hh>

namespace elle
{
  /// Bench a block of code or display statistics about some data.
  ///
  /// N.B. The Bench is active if the LOG_LEVEL related is activated.
  ///
  /// @code{.cc}
  ///
  /// static auto bench = elle::Bench("bench.loop");
  /// for (int i = 0; i < 1000; ++i)
  /// {
  ///   elle::Bench::BenchScope s(bench);
  ///   ::usleep(10);
  /// }
  /// // Result (with ELLE_LOG_LEVEL="bench*:TRACE"):
  /// [bench.loop] AVG: 1162.05 MIN: 1050 MAX: 2578 CNT: 1000 TOT: 1162 ms
  ///
  /// @endcode
  template <typename Type = elle::Duration>
  class ELLE_API Bench
  {
  public:
    using Clock = elle::Clock;
    using Time = Clock::time_point;
    using Duration = Clock::duration;

    /// Construct a Bench.
    ///
    /// @param log_interval If set, bench will automatically log and reset at
    ///                     given interval
    /// @param round The round values to that many digits below 1.
    Bench(std::string name,
          Duration log_interval = {},
          int roundto = 2);
    /// Destroy the Bench.
    ///
    /// This call show() if the component is enabled.
    ~Bench();
    /// Add a value for a given Bench.
    ///
    /// BenchScope automatically adds its lifetime duration to its owner Bench.
    ///
    /// @param val A duration (in milliseconds) to add to the Bench.
    void
    add(Type val);
    /// Reset all underlying values of the Bench.
    void
    reset();
    /// Output the Bench result.
    void
    log();
    /// Output the well-formatted Bench results.
    void
    show();
    /// Pretty print.
    void
    print(std::ostream& os) const;

    template <typename Type_ = Type,
              typename = std::enable_if_t<is_duration<Type_>{}>>
    struct BenchScope
    {
      BenchScope(Bench& owner);
      ~BenchScope();
    private:
      Time _start;
      Bench& _owner;
    };

    template <typename Type_ = Type,
              typename = std::enable_if_t<is_duration<Type_>{}>>
    BenchScope<Type_>
    scoped()
    {
      return *this;
    }

    /// Name of the bench.
    ELLE_ATTRIBUTE_R(std::string, name);
    /// Accumulated values.
    ELLE_ATTRIBUTE_R(Type, sum);
    /// Number of durations recorded.
    ELLE_ATTRIBUTE_R(long, count);
    /// Smallest value.
    ELLE_ATTRIBUTE_R(Type, min);
    /// Greatest value.
    ELLE_ATTRIBUTE_R(Type, max);
    ELLE_ATTRIBUTE_R(Duration, log_interval);
    ELLE_ATTRIBUTE(double, roundfactor);
    ELLE_ATTRIBUTE_R(bool, enabled);
    // Make it last, so that it is set only when the remainder was
    // initialized.
    ELLE_ATTRIBUTE_R(Time, start);
  };
}

#include <elle/bench.hxx>
