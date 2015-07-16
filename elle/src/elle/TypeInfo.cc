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
#ifdef __clang__
    return strcmp(this->_info->name(), rhs._info->name()) == 0;
#else
    return *this->_info == *rhs._info;
#endif
  }

  bool
  TypeInfo::operator <(TypeInfo const& rhs) const
  {
   // When using before on two different typeinfos for the same type,
   // clang fails to treat them as equals.
#ifdef __clang__
    return strcmp(this->_info->name(), rhs._info->name()) < 0;
#else
    return this->_info->before(*rhs._info);
#endif
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
