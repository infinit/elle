//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [wed mar 31 23:32:06 2010]
//

#ifndef NUCLEUS_NEUTRON_RANGE_HH
#define NUCLEUS_NEUTRON_RANGE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/neutron/Subject.hh>
#include <nucleus/neutron/Record.hh>
#include <nucleus/neutron/Size.hh>

#include <elle/idiom/Close.hh>
# include <list>
#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a set of something.
    ///
    /// a range must be parameterised with a type providing two things:
    ///  1) a type T::S which defines the key type used to differenciate
    ///     items.
    ///  2) a Symbol() method which must returns a T::S&, this method
    ///     being used to retrieve the key value of a given item.
    ///
    template <typename T>
    class Range:
      public elle::Object
    {
    public:
      //
      // constants
      //
      static const T*			Trash;

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
      typedef typename T::S				S;

      typedef std::list<T*>				Container;
      typedef typename Container::iterator		Iterator;
      typedef typename Container::const_iterator	Scoutor;

      //
      // constructors & destructors
      //
      Range();
      Range(const Range<T>&);
      ~Range();

      //
      // methods
      //
      elle::Status	Add(T*);
      elle::Status	Exist(const S&) const;
      elle::Status	Lookup(const S&,
			       T*& = Trash) const;
      elle::Status	Remove(const S&);
      elle::Status	Capacity(Size&) const;
      elle::Status	Locate(const S&,
			       Scoutor&) const;
      elle::Status	Locate(const S&,
			       Iterator&);

      elle::Status	Detach();

      //
      // interfaces
      //

      // object
      declare(Range<T>);
      elle::Boolean	operator==(const Range<T>&) const;

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      //
      // attributes
      //
      Options		options;

      Container		container;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <nucleus/neutron/Range.hxx>

#endif
