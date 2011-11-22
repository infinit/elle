//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien.quintard   [tue nov 22 10:32:54 2011]
//

#ifndef ELLE_UTILITY_JSON
#define ELLE_UTILITY_JSON

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/String.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>

#include <elle/idiom/Close.hh>
# include <jsoncpp/json.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;

  namespace utility
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class JSON
    {
    public:
      //
      // classes
      //

      ///
      /// XXX
      ///
      class Node:
	public Object
      {
      public:
	//
	// methods
	//
	Status		Create(const ::Json::Value&);

	//
	// interfaces
	//

	// object
	declare(Node);

	// dumpable
	Status		Dump(const Natural32 = 0) const;

	//
	// attributes
	//
	::Json::Value	value;
      };

      ///
      /// XXX
      ///
      class Document:
	public Object
      {
      public:
	//
	// constants
	//
	static const String		Type;

	//
	// methods
	//
	Status		Load(const String&);

	//
	// interfaces
	//

	// object
	declare(Document);
	Boolean		operator==(const Document&) const;

	// dumpable
	Status		Dump(const Natural32 = 0) const;

	//
	// attributes
	//
	Node		root;
      };
    };

  }
}

#endif
