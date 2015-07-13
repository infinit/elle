#include <elle/TypeInfo.hh>

#include <elle/Backtrace.hh>

namespace elle
{
  std::string
  TypeInfo::name() const
  {
    return elle::demangle(this->_info->name());
  }

  bool
  TypeInfo::operator ==(TypeInfo const& rhs) const
  {
    return *this->_info == *rhs._info;
  }

  bool
  TypeInfo::operator <(TypeInfo const& rhs) const
  {
    return this->_info->before(*rhs._info);
  }

  TypeInfo::TypeInfo(std::type_info const* info)
    : _info(info)
  {}

  std::ostream&
  operator << (std::ostream& s, TypeInfo const& ti)
  {
    s << ti.name();
    return s;
  }
}
