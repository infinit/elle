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

#include <elle/io/Uniquable.hh>

#include <elle/idiom/Close.hh>
# include <jsoncpp/json.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace io;

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
	// enumerations
	//
	enum Type
	  {
	    TypeNull = ::Json::nullValue,
	    TypeInteger = ::Json::intValue,
	    TypeNatural = ::Json::uintValue,
	    TypeReal = ::Json::realValue,
	    TypeString = ::Json::stringValue,
	    TypeBoolean = ::Json::booleanValue,
	    TypeArray = ::Json::arrayValue,
	    TypeObject = ::Json::objectValue
	  };

	//
	// methods
	//
	Status		Create(const ::Json::Value&);

	Status		Type(enum JSON::Node::Type&);

	Status		Get(Node&) const;
	Status		Get(Null&) const;
	Status		Get(Integer8&) const;
	Status		Get(Integer16&) const;
	Status		Get(Integer32&) const;
	Status		Get(Integer64&) const;
	Status		Get(Natural8&) const;
	Status		Get(Natural16&) const;
	Status		Get(Natural32&) const;
	Status		Get(Natural64&) const;
	Status		Get(Real&) const;
	Status		Get(String&) const;
	Status		Get(Boolean&) const;
	Status		Get(const Natural32&,
			    Node&) const;
	Status		Get(const String&,
			    Node&) const;
	Status		Get(Uniquable&) const;

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

	//
	// variadic templates
	//
	template <typename X>
	Status		Get(X&) const;
	template <typename T1,
		  typename X>
	Status		Get(const T1&,
			    X&) const;
	template <typename T1,
		  typename T2,
		  typename X>
	Status		Get(const T1&,
			    const T2&,
			    X&) const;
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename X>
	Status		Get(const T1&,
			    const T2&,
			    const T3&,
			    X&) const;
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename X>
	Status		Get(const T1&,
			    const T2&,
			    const T3&,
			    const T4&,
			    X&) const;
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename X>
	Status		Get(const T1&,
			    const T2&,
			    const T3&,
			    const T4&,
			    const T5&,
			    X&) const;
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6,
		  typename X>
	Status		Get(const T1&,
			    const T2&,
			    const T3&,
			    const T4&,
			    const T5&,
			    const T6&,
			    X&) const;
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6,
		  typename T7,
		  typename X>
	Status		Get(const T1&,
			    const T2&,
			    const T3&,
			    const T4&,
			    const T5&,
			    const T6&,
			    const T7&,
			    X&) const;
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6,
		  typename T7,
		  typename T8,
		  typename X>
	Status		Get(const T1&,
			    const T2&,
			    const T3&,
			    const T4&,
			    const T5&,
			    const T6&,
			    const T7&,
			    const T8&,
			    X&) const;
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6,
		  typename T7,
		  typename T8,
		  typename T9,
		  typename X>
	Status		Get(const T1&,
			    const T2&,
			    const T3&,
			    const T4&,
			    const T5&,
			    const T6&,
			    const T7&,
			    const T8&,
			    const T9&,
			    X&) const;
      };
    };

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{

  ostream&		operator<<(ostream&,
				   const elle::utility::JSON::Document&);
  ostream&		operator<<(ostream&,
				   const elle::utility::JSON::Node&);

}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/utility/JSON.hxx>

#endif
