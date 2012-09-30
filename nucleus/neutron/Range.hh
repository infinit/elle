#ifndef NUCLEUS_NEUTRON_RANGE_HH
# define NUCLEUS_NEUTRON_RANGE_HH

# include <elle/types.hh>

# include <nucleus/neutron/fwd.hh>

# include <elle/idiom/Close.hh>
#  include <list>
# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace neutron
  {

    ///
    /// this class represents a set of something.
    ///
    /// a range must be parameterised with a type providing two things:
    ///  1) a type T::S which defines the key type used to differenciate
    ///     items.
    ///  2) a symbol() method which must returns a T::S&, this method
    ///     being used to retrieve the key value of a given item.
    ///
    template <typename T>
    class Range
    {
    public:
      //
      // constants
      //
      static const T*                   Trash;

      //
      // enumerations
      //
      enum Options
        {
          OptionNone = 0,

          OptionDetach
        };

      //
      // types
      //
      typedef typename T::Symbol                        Symbol;

      typedef std::list<T*>                             Container;
      typedef typename Container::iterator              Iterator;
      typedef typename Container::const_iterator        Scoutor;

      //
      // constructors & destructors
      //
      Range();
      Range(const Range<T>&);
      ~Range();

      //
      // methods
      //
      elle::Status      Add(T*);
      /// XXX
      elle::Status
      Add(Range<T> const& other);
      elle::Status      Exist(const Symbol&) const;
      elle::Status      Lookup(const Symbol&,
                               T const*& = Trash) const;
      elle::Status      Lookup(const Symbol&,
                               T*& = Trash) const;
      elle::Status      Remove(const Symbol&);
      elle::Status      Capacity(Size&) const;
      elle::Status      Locate(const Symbol&,
                               Scoutor&) const;
      elle::Status      Locate(const Symbol&,
                               Iterator&);

      elle::Status      Detach();

      //
      // operators
      //
    public:
      elle::Boolean
      operator ==(Range<T> const& other) const;
      // XXX ELLE_OPERATOR_NEQ_T1(Range<T>);

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Options           options;

      Container         container;
    };

  }
}

#include <nucleus/neutron/Range.hxx>

#endif
