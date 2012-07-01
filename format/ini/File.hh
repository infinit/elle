#ifndef ELLE_FORMAT_INI_FILE_HH
# define ELLE_FORMAT_INI_FILE_HH

# include <unordered_map>

# include <elle/types.hh>

# include <elle/format/ini/Section.hh>

namespace elle { namespace format { namespace ini {

    class File
    {
    private:
      typedef std::unordered_map<elle::String, Section*> Map;
    public:
      typedef Map::value_type value_type;
      typedef Map::iterator iterator;
      typedef Map::const_iterator const_iterator;


    private:
      Map _sections;

    public:
      ~File();

    public:
      Section& operator [](elle::String const& section_name);
      Section const& operator [](elle::String const& section_name) const;
      void Remove(elle::String const& section_name, bool force = false);
      Section const* Contains(elle::String const& section_name) const;

      const_iterator begin() const { return _sections.begin(); }
      const_iterator end() const { return _sections.end(); }
      size_t size() const { return _sections.size(); }
    };

}}}

#endif /* ! FILE_HH */


