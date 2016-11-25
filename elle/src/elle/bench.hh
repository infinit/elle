#ifndef ELLE_BENCH_HH
# define ELLE_BENCH_HH

#include <elle/attribute.hh>
#include <elle/compiler.hh>
#include <elle/time.hh>

namespace elle
{

  /** Bench a block of code or display statistics about some data.
  */
  class ELLE_API Bench
  {
  public:
    /** Contructor.
    * @param log_interval if set, bench will automatically log and reset at
    *        given interval
    * @param round round values to that many digits below 1.
    */
    Bench(std::string const& name,
          boost::posix_time::time_duration log_interval = {},
          int roundto = 2);
    ~Bench();
    void
    add(double val);
    void
    reset();
    void
    log(); ///< log with ELLE_LOG
    void
    show();
    struct BenchScope
    {
      BenchScope(Bench& owner);
      ~BenchScope();
    private:
      boost::posix_time::ptime _start;
      Bench& _owner;
    };

    ELLE_ATTRIBUTE_R(std::string, name);
    ELLE_ATTRIBUTE_R(double, sum);
    ELLE_ATTRIBUTE_R(long, count);
    ELLE_ATTRIBUTE_R(double, min);
    ELLE_ATTRIBUTE_R(double, max);
    ELLE_ATTRIBUTE_R(boost::posix_time::time_duration, log_interval);
    ELLE_ATTRIBUTE(double, roundfactor);
    ELLE_ATTRIBUTE_R(bool, enabled);
    // Make it last, so that it is set only when the remainder was
    // initialized.
    ELLE_ATTRIBUTE_R(boost::posix_time::ptime, start);
  };
}

#endif
