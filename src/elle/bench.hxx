#include <elle/log.hh>
#include <elle/printf.hh>

namespace elle
{
  template <typename Type>
  Bench<Type>::~Bench()
  {
    if (this->_enabled && this->_count)
      this->show();
  }

  template <typename Type>
  Bench<Type>::Bench(std::string name,
               Duration log_interval,
               int roundto)
    : _name(std::move(name))
    , _sum{}
    , _count(0)
    , _min{}
    , _max{}
    , _log_interval(log_interval)
    , _roundfactor(std::pow(10, roundto))
    , _enabled{elle::log::detail::Send::active(elle::log::Logger::Level::trace,
                                               elle::log::Logger::Type::info,
                                               this->_name.c_str())}
    , _start{Clock::now()}
  {}

  template <typename Type>
  void
  Bench<Type>::add(Type val)
  {
    if (!this->_count)
      this->_min = this->_max = val;
    ++this->_count;
    this->_sum += val;
    this->_min = std::min(val, this->_min);
    this->_max = std::max(val, this->_max);
    if (this->_log_interval != Duration()
        && Clock::now() - this->_start > this->_log_interval)
    {
      log();
      reset();
    }
  }

  template <typename Type>
  void
  Bench<Type>::reset()
  {
    this->_sum = this->_min = this->_max = Type{};
    this->_count = 0;
    this->_start = Clock::now();
  }

  template <typename Type>
  void
  Bench<Type>::log()
  {
    char const* _trace_component_ = this->_name.c_str();
    ELLE_TRACE(
      "%s: AVG %s, MIN %s, MAX %s, COUNT %s", this->_name,
      this->_sum / this->_count, this->_min, this->_max, this->_count);
  }

  template <typename Type>
  void
  Bench<Type>::show()
  {
    elle::log::detail::Send send(
      elle::log::Logger::Level::trace,
      elle::log::Logger::Type::info,
      false,
      this->_name,
      __FILE__,
      __LINE__,
      ELLE_COMPILER_PRETTY_FUNCTION,
      // elle::print don't yet support %t (absolute tabulation).
      //  "AVG %12s %16tMIN %16s %32tMAX %12s %48tCNT %12s %64tTOT %8s ms",
      "AVG %s\tMIN %s\tMAX %s\tCNT %s\tTOT %s ms",
      this->_sum / this->_count,
      this->_min, this->_max, this->_count, this->_sum);
  }

  template <typename Type>
  void
  Bench<Type>::print(std::ostream& os) const
  {
    elle::fprintf(os,
      "AVG %12s %16tMIN %16s %32tMAX %12s %48tCNT %12s %64tTOT %8s ms",
      this->_sum / this->_count,
      this->_min, this->_max, this->_count, this->_sum);
  }

  template <typename Type>
  template <typename Type_, typename Enable>
  Bench<Type>::BenchScope<Type_, Enable>::BenchScope(Bench& owner)
    : _owner(owner)
  {
    this->_start = Clock::now();
  }

  template <typename Type>
  template <typename Type_, typename Enable>
  Bench<Type>::BenchScope<Type_, Enable>::~BenchScope()
  {
    this->_owner.add(Clock::now() - this->_start);
  }
}
