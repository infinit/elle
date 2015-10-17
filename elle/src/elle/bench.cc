#include <elle/bench.hh>
#include <elle/log.hh>

namespace elle
{
  Bench::~Bench()
  {
    if (_enabled)
    {
      // log might be already down, play safe
      show(std::cerr);
      std::cerr << std::endl;
    }
  }
  Bench::Bench(std::string const& name,
               boost::posix_time::time_duration log_interval,
               int roundto)
    : _name(name)
    , _sum(0)
    , _count(0)
    , _min(0)
    , _max(0)
    , _log_interval(log_interval)
    , _roundfactor(std::pow(10, roundto))
  {
    _enabled = elle::log::detail::Send::_enabled(elle::log::Logger::Type::info,
      elle::log::Logger::Level::trace, _name.c_str());
    _start = boost::posix_time::microsec_clock::universal_time();
  }

  void Bench::add(double val)
  {
    if (!_count) _min = _max = val;
    ++_count;
    _sum += val;
    _min = std::min (val, _min);
    _max = std::max (val, _max);
    if (_log_interval != boost::posix_time::time_duration()
      && boost::posix_time::microsec_clock::universal_time() - _start
    > _log_interval)
    {
      log();
      reset();
    }
  }
  void Bench::reset()
  {
    _sum=_count=_min=_max=0;
    _start = boost::posix_time::microsec_clock::universal_time();
  }
  void Bench::log()
  {
    char const* _trace_component_ = _name.c_str();
    ELLE_TRACE("%s: AVG %s, MIN %s, MAX %s, COUNT %s", _name,
      std::round(double(_sum*_roundfactor/_count)) / _roundfactor,
      _min, _max, _count);
  }
  void Bench::show(std::ostream& os)
  {
    os << _name << " AVG " << (_sum/_count) << " MIN " << _min << " MAX " << _max << " COUNT " << _count;
  }

  Bench::BenchScope::BenchScope(Bench& owner)
  :_owner(owner)
  {
    _start = boost::posix_time::microsec_clock::universal_time();
  }
  Bench::BenchScope::~BenchScope()
  {
    auto d = boost::posix_time::microsec_clock::universal_time() - _start;
    _owner.add(d.total_microseconds());
  }
}