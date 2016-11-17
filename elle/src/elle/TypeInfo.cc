#ifdef __clang__
# include <cstring>
#endif

#include <elle/TypeInfo.hh>

#include <algorithm>

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
   // clang fails to treat them as equals.
#ifdef __clang__
    return strcmp(this->_info.name(), rhs._info.name()) < 0;
#else
    return this->_info < rhs._info;
#endif
  }

  TypeInfo::TypeInfo(std::type_info const* info)
    : _info(*info)
  {}

  static
  std::vector<std::pair<std::string, std::string>>&
  _abbreviations()
  {
    static std::vector<std::pair<std::string, std::string> > abbreviations;
    return abbreviations;
  }


  TypeInfo::RegisterAbbrevation::RegisterAbbrevation(std::string const& full,
                                                     std::string const& abbr)
  {
    _abbreviations().emplace_back(full, abbr);
    std::sort(_abbreviations().begin(), _abbreviations().end(),
              [] (std::pair<std::string, std::string> const& lhs,
                  std::pair<std::string, std::string> const& rhs)
              {
                return lhs.first.size() > rhs.first.size();
              });
  }

  std::ostream&
  operator << (std::ostream& s, TypeInfo const& ti)
  {
    bool fixed = s.flags() & std::ios::fixed;
    auto name = ti.name();
    if (fixed)
    {
      for (auto const& abbr: _abbreviations())
      {
        auto pos = name.find(abbr.first);
        if (pos != std::string::npos)
          name.replace(pos, abbr.first.size(), abbr.second);
      }
    }
    s << name;
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
