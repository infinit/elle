#ifndef LUNE_MAP_HH
# define LUNE_MAP_HH

# include <elle/types.hh>
# include <elle/operator.hh>

# include <boost/noncopyable.hpp>

namespace lune
{

  ///
  /// this class represents an map of links between local user/group
  /// names and Infinit identifiers.
  ///
  template <typename T>
  class Map:
    private boost::noncopyable
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
    elle::Boolean       Lookup(const elle::String&,
                               T*& = Trash::Value);
    elle::Boolean       Lookup(const T&,
                               elle::String*& = Trash::Name);
    elle::Status        Remove(const elle::String&);

    //
    // interfaces
    //

    ELLE_OPERATOR_NO_ASSIGNMENT(Map);

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
