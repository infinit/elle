//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Range.hh
//
// created       julien quintard   [wed mar 31 23:32:06 2010]
// updated       julien quintard   [fri may 28 17:45:57 2010]
//

#ifndef ETOILE_KERNEL_RANGE_HH
#define ETOILE_KERNEL_RANGE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/kernel/Subject.hh>
#include <etoile/kernel/Record.hh>
#include <etoile/kernel/Size.hh>

#include <list>

namespace etoile
{
  namespace kernel
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a subset of something: more precisely a
    /// collection of things belonging to something else.
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
      elle::Status	Exist(const S&);
      elle::Status	Lookup(const S&,
			       T*& = Trash);
      elle::Status	Remove(const S&);
      elle::Status	Capacity(Size&) const;
      elle::Status	Locate(const S&,
			       Iterator* = NULL);

      elle::Status	Detach();

      //
      // interfaces
      //

      // object
      declare(Range<T>);

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

#include <etoile/kernel/Range.hxx>

#endif
