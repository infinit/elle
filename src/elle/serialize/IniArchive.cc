#include <iostream>

#include <boost/algorithm/string.hpp>

# include <elle/format/ini/Section.hh>
# include <elle/format/ini/File.hh>
# include <elle/format/ini/Section.hxx>

#include "IniArchive.hh"

using namespace elle::serialize;

OutputIniArchive::OutputIniArchive(StreamType& stream)
  : BaseClass(stream)
{}

OutputIniArchive::OutputIniArchive(StreamType& stream, elle::format::ini::File const& ini_file)
  : BaseClass(stream)
{
  *this << ini_file;
}

OutputIniArchive& OutputIniArchive::operator <<(elle::format::ini::Section const& section)
{
  this->stream() << "[" << section.name() << "]\n";
  auto it = section.begin(),
       end = section.end();
  for (; it != end; ++it)
      this->stream() << it->first << " = " << it->second << '\n';
  return *this;
}

OutputIniArchive& OutputIniArchive::operator <<(elle::format::ini::File const& ini_file)
{
  auto it = ini_file.begin(),
       end = ini_file.end();
  for (; it != end; ++it)
    {
      assert(it->second != nullptr);
      *this << *it->second;
    }
  return *this;
}

InputIniArchive::InputIniArchive(StreamType& stream)
  : BaseClass(stream)
{}

InputIniArchive& InputIniArchive::operator >>(elle::format::ini::File& file)
{
  elle::format::ini::Section* section = nullptr;
  std::string line;

  while (std::getline(this->stream(), line))
    {
      boost::trim(line);
      if (line.empty() || line[0] == '#')
        continue;

      if (line[0] == '[')
        {
          auto end = line.find(']');
          if (end == std::string::npos)
            throw std::runtime_error("Couldn't find ']' character");
          auto section_name = boost::trim_copy(line.substr(1, end - 1));
          if (section_name.empty())
            throw std::runtime_error("A section name cannot be empty");
          section = &file[section_name];
        }
      else
        {
          size_t pos;
          if ((pos = line.find('=')) == std::string::npos)
            throw std::runtime_error("No equal sign found in a section");
          std::string name = boost::trim_copy(line.substr(0, pos)),
                      value = boost::trim_copy(line.substr(pos + 1));
          if (name.empty())
            throw std::runtime_error("setting name cannot be empty");
          (*section)[name] = value;
        }
    }
  return *this;
}


