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

#include <elle/core/Boolean.hh>
#include <elle/core/Integer.hh>
#include <elle/core/Natural.hh>
#include <elle/core/Null.hh>
#include <elle/core/Real.hh>
#include <elle/core/String.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>

#include <elle/io/Uniquable.hh>
#include <elle/io/Format.hh>

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
    /// this class contains everything related to the JSON - JavaScript
    /// Object Notation format.
    ///
    class JSON
    {
    public:
      //
      // classes
      //

      ///
      /// this class represents a document node.
      ///
      /// the class can be used either to read or write data. for reading,
      /// the node returned is actually a pointer to the value within the
      /// document. for writing however, a node is created and then
      /// set leading to an insertion or modification.
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

	enum Mode
	  {
	    ModeAllocated,
	    ModeWrapped
	  };

	//
	// constructors & destructors
	//
	Node();
	Node(::Json::Value*);
	Node(const Node&);

	template <typename T>
	Node(const T&);

	~Node();

	//
	// methods
	//
	Status		Type(enum JSON::Node::Type&);

	Status		Wrap(::Json::Value*);

	Status		Get(Null&) const;
	Status		Get(Boolean&) const;
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
	Status		Get(const Natural32&,
			    Node&) const;
	Status		Get(const String&,
			    Node&) const;
	Status		Get(Node&) const;
	template <const Format F>
	Status		Get(Uniquable<F>&) const;

	Status		Set(const Null&);
	Status		Set(const Boolean&);
	Status		Set(const Integer8&);
	Status		Set(const Integer16&);
	Status		Set(const Integer32&);
	Status		Set(const Integer64&);
	Status		Set(const Natural8&);
	Status		Set(const Natural16&);
	Status		Set(const Natural32&);
	Status		Set(const Natural64&);
	Status		Set(const Real&);
	Status		Set(const String&);
	Status		Set(const Natural32&,
			    const Node&);
	Status		Set(const String&,
			    const Node&);
	Status		Set(const Node&);
	template <const Format F>
	Status		Set(const Uniquable<F>&);

	Status		Append(const Node&);
	Status		Append(const Node&,
			       const Node&);
	Status		Append(const Node&,
			       const Node&,
			       const Node&);
	Status		Append(const Node&,
			       const Node&,
			       const Node&,
			       const Node&);
	Status		Append(const Node&,
			       const Node&,
			       const Node&,
			       const Node&,
			       const Node&);
	Status		Append(const Node&,
			       const Node&,
			       const Node&,
			       const Node&,
			       const Node&,
			       const Node&);
	Status		Append(const Node&,
			       const Node&,
			       const Node&,
			       const Node&,
			       const Node&,
			       const Node&,
			       const Node&);
	Status		Append(const Node&,
			       const Node&,
			       const Node&,
			       const Node&,
			       const Node&,
			       const Node&,
			       const Node&,
			       const Node&);
	Status		Append(const Node&,
			       const Node&,
			       const Node&,
			       const Node&,
			       const Node&,
			       const Node&,
			       const Node&,
			       const Node&,
			       const Node&);

	Status		Exist(const Natural32&);
	Status		Exist(const String&);

	//
	// interfaces
	//

	// object
	declare(Node);
	Boolean		operator==(const Node&) const;

	// dumpable
	Status		Dump(const Natural32 = 0) const;

	//
	// attributes
	//
	Mode		mode;
	::Json::Value*	value;
      };

      ///
      /// this class represents the whole JSON document and provides
      /// methods for operating on it in a very easy way.
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

	template <typename X>
	Status		Set(const X&);
	template <typename T1,
		  typename X>
	Status		Set(const T1&,
			    const X&);
	template <typename T1,
		  typename T2,
		  typename X>
	Status		Set(const T1&,
			    const T2&,
			    const X&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename X>
	Status		Set(const T1&,
			    const T2&,
			    const T3&,
			    const X&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename X>
	Status		Set(const T1&,
			    const T2&,
			    const T3&,
			    const T4&,
			    const X&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename X>
	Status		Set(const T1&,
			    const T2&,
			    const T3&,
			    const T4&,
			    const T5&,
			    const X&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6,
		  typename X>
	Status		Set(const T1&,
			    const T2&,
			    const T3&,
			    const T4&,
			    const T5&,
			    const T6&,
			    const X&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6,
		  typename T7,
		  typename X>
	Status		Set(const T1&,
			    const T2&,
			    const T3&,
			    const T4&,
			    const T5&,
			    const T6&,
			    const T7&,
			    const X&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6,
		  typename T7,
		  typename T8,
		  typename X>
	Status		Set(const T1&,
			    const T2&,
			    const T3&,
			    const T4&,
			    const T5&,
			    const T6&,
			    const T7&,
			    const T8&,
			    const X&);
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
	Status		Set(const T1&,
			    const T2&,
			    const T3&,
			    const T4&,
			    const T5&,
			    const T6&,
			    const T7&,
			    const T8&,
			    const T9&,
			    const X&);

	template <typename T1>
	Status		Dig(const T1&,
			    Node&);
	template <typename T1,
		  typename T2>
	Status		Dig(const T1&,
			    const T2&,
			    Node&);
	template <typename T1,
		  typename T2,
		  typename T3>
	Status		Dig(const T1&,
			    const T2&,
			    const T3&,
			    Node&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4>
	Status		Dig(const T1&,
			    const T2&,
			    const T3&,
			    const T4&,
			    Node&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5>
	Status		Dig(const T1&,
			    const T2&,
			    const T3&,
			    const T4&,
			    const T5&,
			    Node&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6>
	Status		Dig(const T1&,
			    const T2&,
			    const T3&,
			    const T4&,
			    const T5&,
			    const T6&,
			    Node&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6,
		  typename T7>
	Status		Dig(const T1&,
			    const T2&,
			    const T3&,
			    const T4&,
			    const T5&,
			    const T6&,
			    const T7&,
			    Node&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6,
		  typename T7,
		  typename T8>
	Status		Dig(const T1&,
			    const T2&,
			    const T3&,
			    const T4&,
			    const T5&,
			    const T6&,
			    const T7&,
			    const T8&,
			    Node&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6,
		  typename T7,
		  typename T8,
		  typename T9>
	Status		Dig(const T1&,
			    const T2&,
			    const T3&,
			    const T4&,
			    const T5&,
			    const T6&,
			    const T7&,
			    const T8&,
			    const T9&,
			    Node&);
      };

      //
      // static methods
      //

      static Status	Encode(const Document&,
			       String&);
      static Status	Decode(const String&,
			       Document&);
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
