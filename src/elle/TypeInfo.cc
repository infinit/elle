#include <elle/TypeInfo.hh>

#ifdef __clang__
# include <cstring>
#endif
#include <algorithm>

#include <boost/range/algorithm/sort.hpp>

#include <elle/Backtrace.hh>

namespace elle
{
  std::string
  TypeInfo::name() const
  {
    return elle::demangle(this->_info.name());
  }

  bool
  TypeInfo::operator ==(TypeInfo const& rhs) const
  {
#ifdef __clang__
    return strcmp(this->_info.name(), rhs._info.name()) == 0;
#else
    return this->_info == rhs._info;
#endif
  }

  bool
  TypeInfo::operator <(TypeInfo const& rhs) const
  {
   // When using before on two different typeinfos for the same type,
   // clang fails to treat them as equal.
#ifdef __clang__
    return strcmp(this->_info.name(), rhs._info.name()) < 0;
#else
    return this->_info < rhs._info;
#endif
  }

  TypeInfo::TypeInfo(std::type_info const* info)
    : _info(*info)
  {}

  namespace
  {
    using Map = std::vector<std::pair<std::string, std::string>>;
    Map&
    _abbreviations()
    {
      static auto res = Map{};
      return res;
    }
  }

  TypeInfo::RegisterAbbrevation::RegisterAbbrevation(std::string const& full,
                                                     std::string const& abbr)
  {
    _abbreviations().emplace_back(full, abbr);
    boost::sort(_abbreviations(),
                [] (auto const& lhs, auto const& rhs)
                {
                  return lhs.first.size() > rhs.first.size();
                });
  }

  std::ostream&
  operator << (std::ostream& s, TypeInfo const& ti)
  {
    if (s.flags() & std::ios::fixed)
    {
      auto name = ti.name();
      for (auto const& abbr: _abbreviations())
      {
        auto pos = name.find(abbr.first);
        if (pos != std::string::npos)
          name.replace(pos, abbr.first.size(), abbr.second);
      }
      s << name;
    }
    else
      s << ti.name();
    return s;
  }
}

namespace std
{
  size_t
  hash<elle::TypeInfo>::operator()(elle::TypeInfo const& info) const
  {
    return std::hash<type_index>()(info._info);
  }
}
