#include <elle/Plugin.hh>

namespace elle
{
  void
  BasePlugin::load() const
  {}

  std::unordered_map<std::string, boost::any>&
  plugins_map()
  {
    static std::unordered_map<std::string, boost::any> value;
    return value;
  }

  std::unordered_map<std::string, boost::any>&
  hooks_map()
  {
    static std::unordered_map<std::string, boost::any> value;
    return value;
  }
}
