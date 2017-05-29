#pragma once

#include <elle/attribute.hh>
#include <elle/compiler.hh>
#include <elle/time.hh>

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
  class ELLE_API Bench
  {
  public:
    using Time = std::chrono::time_point<std::chrono::system_clock>;
    using Duration = Time::duration;

    /// Construct a Bench.
    ///
    /// @param log_interval If set, bench will automatically log and reset at
    ///                     given interval
    /// @param round The round values to that many digits below 1.
    Bench(std::string const& name,
          Duration log_interval = {},
          int roundto = 2);
    /// Destroy the Bench.
    ///
    /// This call show() if the component is enabled.
    ~Bench();
    /// Add a time period (in milliseconds) for a given Bench.
    ///
    /// BenchScope automatically adds its lifetime duration to its owner Bench.
    ///
    /// @param val A duration (in milliseconds) to add to the Bench.
    void
    add(double val);
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
    struct BenchScope
    {
      BenchScope(Bench& owner);
      ~BenchScope();
    private:
      Time _start;
      Bench& _owner;
    };

    ELLE_ATTRIBUTE_R(std::string, name);
    ELLE_ATTRIBUTE_R(double, sum);
    ELLE_ATTRIBUTE_R(long, count);
    ELLE_ATTRIBUTE_R(double, min);
    ELLE_ATTRIBUTE_R(double, max);
    ELLE_ATTRIBUTE_R(Duration, log_interval);
    ELLE_ATTRIBUTE(double, roundfactor);
    ELLE_ATTRIBUTE_R(bool, enabled);
    // Make it last, so that it is set only when the remainder was
    // initialized.
    ELLE_ATTRIBUTE_R(Time, start);
  };
}
