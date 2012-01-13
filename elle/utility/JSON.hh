//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue nov 22 10:32:54 2011]
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
# include <vector>
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
    /// below is an example of how to use the JSON-related classes.
    ///
    ///   JSON::Document	document;
    ///   String		string;
    ///
    ///   document.Set("some", "string");
    ///   document.Set("somewhere", "array",
    ///                JSON::Bulk(JSON::Node(42),
    ///                           JSON::Node(21.84),
    ///                           JSON::Node("bande")));
    ///   document.Append("somewhere", "array",
    ///                   JSON::Bulk(JSON::Node("test")));
    ///
    ///   JSON::Encode(document, string);
    ///
    ///   std::cout << string << std::endl;
    ///
    ///   JSON::Decode(string, document);
    ///
    ///   document.Dump();
    ///
    ///   std::cout << document.Get("somewhere", "array", 2) << std::endl;
    ///
    class JSON
    {
    public:
      //
      // forward declarations
      //
      class Node;

      //
      // classes
      //

      ///
      /// this class aims at gathering together nodes which must be
      /// treated handled through a single operation.
      ///
      class Bulk:
	public Object
      {
      public:
	//
	// types
	//
	typedef std::vector<Node>			Container;
	typedef typename Container::iterator		Iterator;
	typedef typename Container::const_iterator	Scoutor;

	//
	// constructors & destructors
	//
	Bulk(const Node&);
	Bulk(const Node&,
	     const Node&);
	Bulk(const Node&,
	     const Node&,
	     const Node&);
	Bulk(const Node&,
	     const Node&,
	     const Node&,
	     const Node&);
	Bulk(const Node&,
	     const Node&,
	     const Node&,
	     const Node&,
	     const Node&);
	Bulk(const Node&,
	     const Node&,
	     const Node&,
	     const Node&,
	     const Node&,
	     const Node&);
	Bulk(const Node&,
	     const Node&,
	     const Node&,
	     const Node&,
	     const Node&,
	     const Node&,
	     const Node&);
	Bulk(const Node&,
	     const Node&,
	     const Node&,
	     const Node&,
	     const Node&,
	     const Node&,
	     const Node&,
	     const Node&);
	Bulk(const Node&,
	     const Node&,
	     const Node&,
	     const Node&,
	     const Node&,
	     const Node&,
	     const Node&,
	     const Node&,
	     const Node&);

	//
	// attributes
	//
	Container	container;
      };

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

	Status		Append(const Bulk&);

	Status		Erase(const String&);

	Status		Clear();

	Status		Size(Natural32&);

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

	Status		Append(const Bulk&);
	template <typename T1>
	Status		Append(const T1&,
			       const Bulk&);
	template <typename T1,
		  typename T2>
	Status		Append(const T1&,
			       const T2&,
			       const Bulk&);
	template <typename T1,
		  typename T2,
		  typename T3>
	Status		Append(const T1&,
			       const T2&,
			       const T3&,
			       const Bulk&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4>
	Status		Append(const T1&,
			       const T2&,
			       const T3&,
			       const T4&,
			       const Bulk&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5>
	Status		Append(const T1&,
			       const T2&,
			       const T3&,
			       const T4&,
			       const T5&,
			       const Bulk&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6>
	Status		Append(const T1&,
			       const T2&,
			       const T3&,
			       const T4&,
			       const T5&,
			       const T6&,
			       const Bulk&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6,
		  typename T7>
	Status		Append(const T1&,
			       const T2&,
			       const T3&,
			       const T4&,
			       const T5&,
			       const T6&,
			       const T7&,
			       const Bulk&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6,
		  typename T7,
		  typename T8>
	Status		Append(const T1&,
			       const T2&,
			       const T3&,
			       const T4&,
			       const T5&,
			       const T6&,
			       const T7&,
			       const T8&,
			       const Bulk&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6,
		  typename T7,
		  typename T8,
		  typename T9>
	Status		Append(const T1&,
			       const T2&,
			       const T3&,
			       const T4&,
			       const T5&,
			       const T6&,
			       const T7&,
			       const T8&,
			       const T9&,
			       const Bulk&);

	template <typename T1>
	Status		Erase(const T1&);
	template <typename T1,
		  typename T2>
	Status		Erase(const T1&,
			      const T2&);
	template <typename T1,
		  typename T2,
		  typename T3>
	Status		Erase(const T1&,
			      const T2&,
			      const T3&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4>
	Status		Erase(const T1&,
			      const T2&,
			      const T3&,
			      const T4&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5>
	Status		Erase(const T1&,
			      const T2&,
			      const T3&,
			      const T4&,
			      const T5&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6>
	Status		Erase(const T1&,
			      const T2&,
			      const T3&,
			      const T4&,
			      const T5&,
			      const T6&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6,
		  typename T7>
	Status		Erase(const T1&,
			      const T2&,
			      const T3&,
			      const T4&,
			      const T5&,
			      const T6&,
			      const T7&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6,
		  typename T7,
		  typename T8>
	Status		Erase(const T1&,
			      const T2&,
			      const T3&,
			      const T4&,
			      const T5&,
			      const T6&,
			      const T7&,
			      const T8&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6,
		  typename T7,
		  typename T8,
		  typename T9>
	Status		Erase(const T1&,
			      const T2&,
			      const T3&,
			      const T4&,
			      const T5&,
			      const T6&,
			      const T7&,
			      const T8&,
			      const T9&);

	Status		Clear();
	template <typename T1>
	Status		Clear(const T1&);
	template <typename T1,
		  typename T2>
	Status		Clear(const T1&,
			      const T2&);
	template <typename T1,
		  typename T2,
		  typename T3>
	Status		Clear(const T1&,
			      const T2&,
			      const T3&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4>
	Status		Clear(const T1&,
			      const T2&,
			      const T3&,
			      const T4&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5>
	Status		Clear(const T1&,
			      const T2&,
			      const T3&,
			      const T4&,
			      const T5&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6>
	Status		Clear(const T1&,
			      const T2&,
			      const T3&,
			      const T4&,
			      const T5&,
			      const T6&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6,
		  typename T7>
	Status		Clear(const T1&,
			      const T2&,
			      const T3&,
			      const T4&,
			      const T5&,
			      const T6&,
			      const T7&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6,
		  typename T7,
		  typename T8>
	Status		Clear(const T1&,
			      const T2&,
			      const T3&,
			      const T4&,
			      const T5&,
			      const T6&,
			      const T7&,
			      const T8&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6,
		  typename T7,
		  typename T8,
		  typename T9>
	Status		Clear(const T1&,
			      const T2&,
			      const T3&,
			      const T4&,
			      const T5&,
			      const T6&,
			      const T7&,
			      const T8&,
			      const T9&);

	Status		Size(Natural32&);
	template <typename T1>
	Status		Size(const T1&,
			     Natural32&);
	template <typename T1,
		  typename T2>
	Status		Size(const T1&,
			     const T2&,
			     Natural32&);
	template <typename T1,
		  typename T2,
		  typename T3>
	Status		Size(const T1&,
			     const T2&,
			     const T3&,
			     Natural32&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4>
	Status		Size(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     Natural32&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5>
	Status		Size(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     Natural32&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6>
	Status		Size(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     const T6&,
			     Natural32&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6,
		  typename T7>
	Status		Size(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     const T6&,
			     const T7&,
			     Natural32&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6,
		  typename T7,
		  typename T8>
	Status		Size(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     const T6&,
			     const T7&,
			     const T8&,
			     Natural32&);
	template <typename T1,
		  typename T2,
		  typename T3,
		  typename T4,
		  typename T5,
		  typename T6,
		  typename T7,
		  typename T8,
		  typename T9>
	Status		Size(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     const T6&,
			     const T7&,
			     const T8&,
			     const T9&,
			     Natural32&);
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
