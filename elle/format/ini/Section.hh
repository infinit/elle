#ifndef ELLE_FORMAT_INI_SECTION_HH
# define ELLE_FORMAT_INI_SECTION_HH

# include <utility>
# include <unordered_map>

# include <elle/types.hh>

namespace elle { namespace format { namespace ini {

    class Section
    {
    private:
      typedef std::unordered_map<elle::String, elle::String> Map;
    public:
      typedef Map::value_type value_type;
      typedef Map::iterator iterator;
      typedef Map::const_iterator const_iterator;

    private:
      elle::String _name;
      Map _pairs;

    public:
      Section(elle::String const& name);

    public:
      elle::String const* Contains(elle::String const& key) const;
      template<typename T> T Get(elle::String const& key) const;
      template<typename T> T Get(elle::String const& key, T const& default_value) const;
      template<typename T> void Set(elle::String const& key, T const& value);
      elle::String const& operator [](elle::String const& key) const;
      elle::String& operator [](elle::String const& key);

      elle::String const& name() const { return _name; }

      const_iterator begin() const { return _pairs.begin(); }
      const_iterator end() const { return _pairs.end(); }
      size_t size() const { return _pairs.size(); }
    };

}}}

# include <elle/idiom/Close.hh>
# include "Section.hxx"
# include <elle/idiom/Open.hh>

#endif /* ! SECTION_HH */


