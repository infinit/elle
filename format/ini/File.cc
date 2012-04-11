
#include <stdexcept>

#include "File.hh"

using namespace elle::format::ini;

File::~File()
{
  auto it = _sections.begin(),
       end = _sections.end();
  for (; it != end; ++it)
    {
      assert(it->second != nullptr);
      delete it->second;
    }
  _sections.clear();
}

Section& File::operator [](elle::String const& section_name)
{
  auto it = _sections.find(section_name);
  Section* section = nullptr;
  if (it == _sections.end())
    {
      section = new Section(section_name);
      _sections.insert({{section_name, section}});
    }
  else
    section = it->second;
  assert(section != nullptr);
  return *section;
}

Section const& File::operator [](elle::String const& section_name) const
{
  auto it = _sections.find(section_name);
  if (it != _sections.end())
    {
      assert(it->second != nullptr);
      return *it->second;
    }
  throw std::runtime_error("KeyError: '" + section_name + "'");
}

void File::Remove(elle::String const& section_name, bool force)
{
  if (!force && _sections.find(section_name) == _sections.end())
      throw std::runtime_error("KeyError '" + section_name + "'");
  _sections.erase(section_name);
}

Section const* File::Contains(elle::String const& section_name) const
{
  auto it = _sections.find(section_name);
  if (it != _sections.end())
    {
      assert(it->second != nullptr);
      return it->second;
    }
  return nullptr;
}
