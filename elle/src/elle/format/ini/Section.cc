
#include <stdexcept>

# include "Section.hh"

using namespace elle::format::ini;

Section::Section(elle::String const& name)
  : _name(name)
  , _pairs()
{}

elle::String const* Section::Contains(elle::String const& key) const
{
  auto it = _pairs.find(key);
  if (it != _pairs.end())
    return &it->second;
  return nullptr;
}

elle::String const& Section::operator [](elle::String const& key) const
{
  auto it = _pairs.find(key);
  if (it != _pairs.end())
    return it->second;
  throw std::runtime_error("KeyError: '" + key + "'");
}

elle::String& Section::operator [](elle::String const& key)
{
  return _pairs[key];
}
