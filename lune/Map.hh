#ifndef LUNE_MAP_HH
# define LUNE_MAP_HH

# include <elle/types.hh>
# include <elle/idiom/Open.hh>

namespace lune
{

  ///
  /// this class represents an map of links between local user/group
  /// names and Infinit identifiers.
  ///
  template <typename T>
  class Map:
    public elle::radix::Object
  {
  public:
    //
    // constants
    //
    struct                      Trash
    {
      static elle::String*      Name;
      static T*                 Value;
    };

    //
    // structures
    //
    struct Entry
    {
      //
      // attributes
      //
      elle::String      name;
      T                 value;
    };

    //
    // types
    //
    typedef std::list<Entry*>                           Container;
    typedef typename Container::iterator                Iterator;
    typedef typename Container::const_iterator          Scoutor;

    //
    // constructors & destructors
    //
    ~Map();

    //
    // methods
    //
    elle::Status        Add(const elle::String&,
                            const T&);
    elle::Status        Lookup(const elle::String&,
                               T*& = Trash::Value);
    elle::Status        Lookup(const T&,
                               elle::String*& = Trash::Name);
    elle::Status        Remove(const elle::String&);

    //
    // interfaces
    //

    // object
    declare(Map);

    // dumpable
    elle::Status        Dump(const elle::Natural32 = 0) const;

    //
    // attributes
    //
    Container           container;
  };

}

# include <lune/Map.hxx>

#endif
