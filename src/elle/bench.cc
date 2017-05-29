#include <elle/bench.hh>

#include <elle/log.hh>
#include <elle/printf.hh>

namespace
{
  auto now()
  {
    return std::chrono::system_clock::now();
  }
}

namespace elle
{
  Bench::~Bench()
  {
    if (this->_enabled && this->_count)
      this->show();
  }

  Bench::Bench(std::string const& name,
               Duration log_interval,
               int roundto)
    : _name(name)
    , _sum(0)
    , _count(0)
    , _min(0)
    , _max(0)
    , _log_interval(log_interval)
    , _roundfactor(std::pow(10, roundto))
    , _enabled{elle::log::detail::Send::active(elle::log::Logger::Level::trace,
                                               elle::log::Logger::Type::info,
                                               this->_name.c_str())}
    , _start{now()}
  {}

  void
  Bench::add(double val)
  {
    if (!this->_count)
      this->_min = this->_max = val;
    ++this->_count;
    this->_sum += val;
    this->_min = std::min(val, this->_min);
    this->_max = std::max(val, this->_max);
    if (this->_log_interval != Duration()
        && now() - this->_start > this->_log_interval)
    {
      log();
      reset();
    }
  }

  void
  Bench::reset()
  {
    this->_sum = this->_count = this->_min = this->_max = 0;
    this->_start = now();
  }

  void
  Bench::log()
  {
    char const* _trace_component_ = this->_name.c_str();
    ELLE_TRACE(
      "%s: AVG %s, MIN %s, MAX %s, COUNT %s", this->_name,
      std::round(double(this->_sum *
                        this->_roundfactor /
                        this->_count)) / this->_roundfactor,
      this->_min, this->_max, this->_count);
  }

  void Bench::show()
  {
    elle::log::detail::Send send(
      elle::log::Logger::Level::trace,
      elle::log::Logger::Type::info,
      false,
      this->_name,
      __FILE__,
      __LINE__,
      ELLE_COMPILER_PRETTY_FUNCTION,
      "AVG %s\tMIN %s\tMAX %s\tCNT %s\tTOT %s ms",
      this->_sum / this->_count,
      this->_min,
      this->_max,
      this->_count,
      uint64_t(this->_sum) / 1000u);
  }

  void
  Bench::print(std::ostream& os) const
  {
    elle::fprintf(os,
      "AVG %12s %16tMIN %16s %32tMAX %12s %48tCNT %12s %64tTOT %8s ms",
      this->_sum / this->_count,
      this->_min,
      this->_max,
      this->_count,
      uint64_t(this->_sum) / 1000u);
  }

  Bench::BenchScope::BenchScope(Bench& owner)
    : _owner(owner)
  {
    this->_start = now();
  }

  Bench::BenchScope::~BenchScope()
  {
    using ms = std::chrono::microseconds;
    auto const d = std::chrono::duration_cast<ms>(now() - this->_start);
    this->_owner.add(d.count());
  }
}
