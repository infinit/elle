#include <elle/Plugin.hh>

namespace elle
{
  void
  BasePlugin::load() const
  {}

  std::unordered_map<elle::TypeInfo, boost::any>&
  plugins_map()
  {
    static std::unordered_map<elle::TypeInfo, boost::any> value;
    return value;
  }

  std::unordered_map<elle::TypeInfo, boost::any>&
  hooks_map()
  {
    static std::unordered_map<elle::TypeInfo, boost::any> value;
    return value;
  }
}
