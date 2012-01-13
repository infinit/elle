//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue nov 22 12:50:06 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/utility/JSON.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/io/Unique.hh>

#include <elle/idiom/Close.hh>
# include <fstream>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace standalone;

  namespace utility
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this constant defines the JSON type.
    ///
    const String		JSON::Document::Type = "application/json";

//
// ---------- bulk ------------------------------------------------------------
//

    ///
    /// these constructors take any number of nodes so as to be treated
    /// as a whole.
    ///

    JSON::Bulk::Bulk(const Node&				node)
    {
      // record the node.
      this->container.push_back(node);
    }

    JSON::Bulk::Bulk(const Node&				node1,
		     const Node&				node2)
    {
      // record the nodes.
      this->container.push_back(node1);
      this->container.push_back(node2);
    }

    JSON::Bulk::Bulk(const Node&				node1,
		     const Node&				node2,
		     const Node&				node3)
    {
      // record the nodes.
      this->container.push_back(node1);
      this->container.push_back(node2);
      this->container.push_back(node3);
    }

    JSON::Bulk::Bulk(const Node&				node1,
		     const Node&				node2,
		     const Node&				node3,
		     const Node&				node4)
    {
      // record the nodes.
      this->container.push_back(node1);
      this->container.push_back(node2);
      this->container.push_back(node3);
      this->container.push_back(node4);
    }

    JSON::Bulk::Bulk(const Node&				node1,
		     const Node&				node2,
		     const Node&				node3,
		     const Node&				node4,
		     const Node&				node5)
    {
      // record the nodes.
      this->container.push_back(node1);
      this->container.push_back(node2);
      this->container.push_back(node3);
      this->container.push_back(node4);
      this->container.push_back(node5);
    }

    JSON::Bulk::Bulk(const Node&				node1,
		     const Node&				node2,
		     const Node&				node3,
		     const Node&				node4,
		     const Node&				node5,
		     const Node&				node6)
    {
      // record the nodes.
      this->container.push_back(node1);
      this->container.push_back(node2);
      this->container.push_back(node3);
      this->container.push_back(node4);
      this->container.push_back(node5);
      this->container.push_back(node6);
    }

    JSON::Bulk::Bulk(const Node&				node1,
		     const Node&				node2,
		     const Node&				node3,
		     const Node&				node4,
		     const Node&				node5,
		     const Node&				node6,
		     const Node&				node7)
    {
      // record the nodes.
      this->container.push_back(node1);
      this->container.push_back(node2);
      this->container.push_back(node3);
      this->container.push_back(node4);
      this->container.push_back(node5);
      this->container.push_back(node6);
      this->container.push_back(node7);
    }

    JSON::Bulk::Bulk(const Node&				node1,
		     const Node&				node2,
		     const Node&				node3,
		     const Node&				node4,
		     const Node&				node5,
		     const Node&				node6,
		     const Node&				node7,
		     const Node&				node8)
    {
      // record the nodes.
      this->container.push_back(node1);
      this->container.push_back(node2);
      this->container.push_back(node3);
      this->container.push_back(node4);
      this->container.push_back(node5);
      this->container.push_back(node6);
      this->container.push_back(node7);
      this->container.push_back(node8);
    }

    JSON::Bulk::Bulk(const Node&				node1,
		     const Node&				node2,
		     const Node&				node3,
		     const Node&				node4,
		     const Node&				node5,
		     const Node&				node6,
		     const Node&				node7,
		     const Node&				node8,
		     const Node&				node9)
    {
      // record the nodes.
      this->container.push_back(node1);
      this->container.push_back(node2);
      this->container.push_back(node3);
      this->container.push_back(node4);
      this->container.push_back(node5);
      this->container.push_back(node6);
      this->container.push_back(node7);
      this->container.push_back(node8);
      this->container.push_back(node9);
    }

//
// ---------- node ------------------------------------------------------------
//

    ///
    /// the default constructor sets the node so as to receive data
    /// by allocating a value.
    ///
    JSON::Node::Node():
      value(NULL)
    {
    }

    ///
    /// this constructor wraps the given value.
    ///
    JSON::Node::Node(::json_object*				object):
      value(::json_object_get(object))
    {
    }

    ///
    /// the copy constructor.
    ///
    JSON::Node::Node(const Node&				node):
      Object(node),

      value(::json_object_get(node.value))
    {
    }

    ///
    /// the destructor.
    ///
    JSON::Node::~Node()
    {
      // release the value.
      ::json_object_put(this->value);
    }

    ///
    /// this method returns the type of the node.
    ///
    Status		JSON::Node::Type(enum JSON::Node::Type&	type)
    {
      enter();

      // return the type.
      type =
	static_cast<enum JSON::Node::Type>(
	  ::json_object_get_type(this->value));

      leave();
    }

    ///
    /// this method returns a null value.
    ///
    Status		JSON::Node::Get(Null&) const
    {
      enter();

      // check the value.
      if (this->value != NULL)
	escape("invalid type: node's(%u) expected(NULL pointer)",
	       ::json_object_get_type(this->value));

      leave();
    }

    ///
    /// this method returns a boolean value.
    ///
    Status		JSON::Node::Get(Boolean&		element) const
    {
      enter();

      // check the type.
      if (::json_object_is_type(this->value, ::json_type_boolean) == 0)
	escape("invalid type: node's(%u) expected(%u)",
	       ::json_object_get_type(this->value), ::json_type_boolean);

      // retrieve the value.
      element = ::json_object_get_boolean(this->value);

      leave();
    }

    ///
    /// this method returns a integer value.
    ///
    Status		JSON::Node::Get(Integer8&		element) const
    {
      enter();

      // check the type.
      if (::json_object_is_type(this->value, ::json_type_int) == 0)
	escape("invalid type: node's(%u) expected(%u)",
	       ::json_object_get_type(this->value), ::json_type_int);

      // retrieve the value.
      element = ::json_object_get_int(this->value);

      leave();
    }

    ///
    /// this method returns a integer value.
    ///
    Status		JSON::Node::Get(Integer16&		element) const
    {
      enter();

      // check the type.
      if (::json_object_is_type(this->value, ::json_type_int) == 0)
	escape("invalid type: node's(%u) expected(%u)",
	       ::json_object_get_type(this->value), ::json_type_int);

      // retrieve the value.
      element = ::json_object_get_int(this->value);

      leave();
    }

    ///
    /// this method returns a integer value.
    ///
    Status		JSON::Node::Get(Integer32&		element) const
    {
      enter();

      // check the type.
      if (::json_object_is_type(this->value, ::json_type_int) == 0)
	escape("invalid type: node's(%u) expected(%u)",
	       ::json_object_get_type(this->value), ::json_type_int);

      // retrieve the value.
      element = ::json_object_get_int(this->value);

      leave();
    }

    ///
    /// this method returns a integer value.
    ///
    Status		JSON::Node::Get(Integer64&) const
    {
      enter();

      escape("64-bit numbers are not supported");
    }

    ///
    /// this method returns a natural value.
    ///
    Status		JSON::Node::Get(Natural8&		element) const
    {
      enter();

      // check the type.
      if (::json_object_is_type(this->value, ::json_type_int) == 0)
	escape("invalid type: node's(%u) expected(%u)",
	       ::json_object_get_type(this->value), ::json_type_int);

      // retrieve the value.
      element = ::json_object_get_int(this->value);

      leave();
    }

    ///
    /// this method returns a natural value.
    ///
    Status		JSON::Node::Get(Natural16&		element) const
    {
      enter();

      // check the type.
      if (::json_object_is_type(this->value, ::json_type_int) == 0)
	escape("invalid type: node's(%u) expected(%u)",
	       ::json_object_get_type(this->value), ::json_type_int);

      // retrieve the value.
      element = ::json_object_get_int(this->value);

      leave();
    }

    ///
    /// this method returns a natural value.
    ///
    Status		JSON::Node::Get(Natural32&		element) const
    {
      enter();

      // check the type.
      if (::json_object_is_type(this->value, ::json_type_int) == 0)
	escape("invalid type: node's(%u) expected(%u)",
	       ::json_object_get_type(this->value), ::json_type_int);

      // retrieve the value.
      element = ::json_object_get_int(this->value);

      leave();
    }

    ///
    /// this method returns a natural value.
    ///
    Status		JSON::Node::Get(Natural64&) const
    {
      enter();

      escape("64-bit numbers are not supported");
    }

    ///
    /// this method returns a real value.
    ///
    Status		JSON::Node::Get(Real&			element) const
    {
      enter();

      // check the type.
      if (::json_object_is_type(this->value, ::json_type_double) == 0)
	escape("invalid type: node's(%u) expected(%u)",
	       ::json_object_get_type(this->value), ::json_type_double);

      // retrieve the value.
      element = ::json_object_get_double(this->value);

      leave();
    }

    ///
    /// this method returns a string value.
    ///
    Status		JSON::Node::Get(String&			element) const
    {
      enter();

      // check the type.
      if (::json_object_is_type(this->value, ::json_type_string) == 0)
	escape("invalid type: node's(%u) expected(%u)",
	       ::json_object_get_type(this->value), ::json_type_string);

      // retrieve the value.
      element = ::json_object_get_string(this->value);

      leave();
    }

    ///
    /// this method returns the node at the given index in the array.
    ///
    Status		JSON::Node::Get(const Natural32&	index,
					Node&			node) const
    {
      enter();

      // check the type.
      if (::json_object_is_type(this->value, ::json_type_array) == 0)
	escape("invalid type: node's(%u) expected(%u)",
	       ::json_object_get_type(this->value), ::json_type_array);

      // check if the index is valid.
      if (index >=
	  static_cast<Natural32>(::json_object_array_length(this->value)))
	escape("the given index '%u' seems to be out-of-bound given the "
	       "size of the array '%u'",
	       index,
	       static_cast<Natural32>(
		 ::json_object_array_length(this->value)));

      // retrieve the value.
      node.value = ::json_object_array_get_idx(this->value, index);

      // check the error.
      if (is_error(node.value))
	escape("unable to retrieve the '%u'-index element",
	       index);

      leave();
    }

    ///
    /// this method returns the node at the given key in the object.
    ///
    Status		JSON::Node::Get(const String&		key,
					Node&			node) const
    {
      enter();

      // check the type.
      if (::json_object_is_type(this->value, ::json_type_object) == 0)
	escape("invalid type: node's(%u) expected(%u)",
	       ::json_object_get_type(this->value), ::json_type_object);

      // retrieve the value.
      node.value = ::json_object_object_get(this->value, key.c_str());

      // check the error.
      if (is_error(node.value))
	escape("unable to retrieve the '%s'-key element",
	       key.c_str());

      leave();
    }

    ///
    /// this method returns the current node.
    ///
    Status		JSON::Node::Get(Node&			node) const
    {
      enter();

      // duplicate the reference on the value.
      node.value = ::json_object_get(this->value);
      
      // check the error.
      if (is_error(node.value))
	escape("unable to duplicate a reference on the object");

      leave();
    }

    ///
    /// this method records a null value.
    ///
    Status		JSON::Node::Set(const Null&)
    {
      enter();

      // release the previous value, if present.
      if (this->value != NULL)
	::json_object_put(this->value);

      // set the value.
      this->value = NULL;

      leave();
    }

    ///
    /// this method records a boolean value.
    ///
    Status		JSON::Node::Set(const Boolean&		element)
    {
      enter();

      // release the previous value, if present.
      if (this->value != NULL)
	::json_object_put(this->value);

      // set the value.
      this->value = ::json_object_new_boolean(element);

      leave();
    }

    ///
    /// this method records a integer value.
    ///
    Status		JSON::Node::Set(const Integer8&		element)
    {
      enter();

      // release the previous value, if present.
      if (this->value != NULL)
	::json_object_put(this->value);

      // set the value.
      this->value = ::json_object_new_int(element);

      leave();
    }

    ///
    /// this method records a integer value.
    ///
    Status		JSON::Node::Set(const Integer16&	element)
    {
      enter();

      // release the previous value, if present.
      if (this->value != NULL)
	::json_object_put(this->value);

      // set the value.
      this->value = ::json_object_new_int(element);

      leave();
    }

    ///
    /// this method records a integer value.
    ///
    Status		JSON::Node::Set(const Integer32&	element)
    {
      enter();

      // release the previous value, if present.
      if (this->value != NULL)
	::json_object_put(this->value);

      // set the value.
      this->value = ::json_object_new_int(element);

      leave();
    }

    ///
    /// this method records a integer value.
    ///
    Status		JSON::Node::Set(const Integer64&)
    {
      enter();

      escape("64-bit numbers are not supported");
    }

    ///
    /// this method records a natural value.
    ///
    Status		JSON::Node::Set(const Natural8&		element)
    {
      enter();

      // release the previous value, if present.
      if (this->value != NULL)
	::json_object_put(this->value);

      // set the value.
      this->value = ::json_object_new_int(element);

      leave();
    }

    ///
    /// this method records a natural value.
    ///
    Status		JSON::Node::Set(const Natural16&	element)
    {
      enter();

      // release the previous value, if present.
      if (this->value != NULL)
	::json_object_put(this->value);

      // set the value.
      this->value = ::json_object_new_int(element);

      leave();
    }

    ///
    /// this method records a natural value.
    ///
    Status		JSON::Node::Set(const Natural32&	element)
    {
      enter();

      // release the previous value, if present.
      if (this->value != NULL)
	::json_object_put(this->value);

      // set the value.
      this->value = ::json_object_new_int(element);

      leave();
    }

    ///
    /// this method records a natural value.
    ///
    Status		JSON::Node::Set(const Natural64&)
    {
      enter();

      escape("64-bit numbers are not supported");
    }

    ///
    /// this method records a real value.
    ///
    Status		JSON::Node::Set(const Real&		element)
    {
      enter();

      // release the previous value, if present.
      if (this->value != NULL)
	::json_object_put(this->value);

      // set the value.
      this->value = ::json_object_new_double(element);

      leave();
    }

    ///
    /// this method records a string value.
    ///
    Status		JSON::Node::Set(const String&		element)
    {
      enter();

      // release the previous value, if present.
      if (this->value != NULL)
	::json_object_put(this->value);

      // set the value.
      this->value = ::json_object_new_string(element.c_str());

      leave();
    }

    ///
    /// this method records a node in the array, at the given index.
    ///
    Status		JSON::Node::Set(const Natural32&	index,
					const Node&		node)
    {
      enter();

      // check the type.
      if (::json_object_is_type(this->value, ::json_type_array) == 0)
	{
	  // release the value.
	  ::json_object_put(this->value);

	  // allocate an array value.
	  this->value = ::json_object_new_array();
	}

      // set the value.
      if (::json_object_array_put_idx(this->value, index, node.value) != 0)
	escape("unable to add the index element to the array");

      leave();
    }

    ///
    /// this method records a node in the object, at the given key.
    ///
    Status		JSON::Node::Set(const String&		key,
					const Node&		node)
    {
      enter();

      // check the type.
      if (::json_object_is_type(this->value, ::json_type_object) == 0)
	{
	  // release the value.
	  ::json_object_put(this->value);

	  // allocate an object value.
	  this->value = ::json_object_new_object();
	}

      // set the value.
      ::json_object_object_add(this->value, key.c_str(), node.value);

      leave();
    }

    ///
    /// this method records a node.
    ///
    Status		JSON::Node::Set(const Node&		node)
    {
      enter();

      // set the value by creating a new reference.
      this->value = ::json_object_get(node.value);

      // check the error.
      if (is_error(this->value))
	escape("unable to duplicate a reference on the object");

      leave();
    }

    ///
    /// this method appends a bulk of nodes to the array.
    ///
    Status		JSON::Node::Append(const Bulk&		bulk)
    {
      JSON::Bulk::Scoutor	scoutor;

      enter();

      // check the type.
      if (!((this->value != NULL) &&
	    (::json_object_is_type(this->value, ::json_type_array) != 0)))
	escape("unable to append to a non-array or non-null node");

      // go through the bulk.
      for (scoutor = bulk.container.begin();
	   scoutor != bulk.container.end();
	   scoutor++)
	{
	  // append the node's value.
	  if (::json_object_array_add(this->value, scoutor->value) != 0)
	    escape("unable to append an element to the array");
	}

      leave();
    }

    ///
    /// this method erases the named member from the object.
    ///
    Status		JSON::Node::Erase(const String&		key)
    {
      enter();

      // check the type.
      if (::json_object_is_type(this->value, ::json_type_object) == 0)
	escape("unable to erase from a non-object node");

      // erase the member.
      ::json_object_object_del(this->value, key.c_str());

      leave();
    }

    ///
    /// this method clears the array or object from all their elements.
    ///
    Status		JSON::Node::Clear()
    {
      enter();

      // depending on the type.
      switch (::json_object_get_type(this->value))
	{
	case ::json_type_array:
	  {
	    // release the value.
	    ::json_object_put(this->value);

	    // allocate a new one.
	    this->value = ::json_object_new_array();

	    break;
	  }
	case ::json_type_object:
	  {
	    // release the object.
	    ::json_object_put(this->value);

	    // allocate a new one.
	    this->value = ::json_object_new_object();

	    break;
	  }
	default:
	  {
	    escape("unable to clear from a non-array or non-object node");
	  }
	}

      leave();
    }

    ///
    /// this method returns the number of elements in the node's array.
    ///
    Status		JSON::Node::Size(Natural32&		size)
    {
      enter();

      // check the type.
      if (::json_object_is_type(this->value, ::json_type_array) == 0)
	escape("unable to size from a non-array node");

      // return the number of elements.
      size = ::json_object_array_length(this->value);

      leave();
    }

    ///
    /// this method returns true if the given index is valid within
    /// the array.
    ///
    Status		JSON::Node::Exist(const Natural32&	index)
    {
      enter();

      // check the type.
      if (::json_object_is_type(this->value, ::json_type_array) == 0)
	false();

      // check if the index is valid.
      if (index >=
	  static_cast<Natural32>(::json_object_array_length(this->value)))
	false();

      true();
    }

    ///
    /// this method returns true if the given key is valid within the
    /// object.
    ///
    Status		JSON::Node::Exist(const String&		key)
    {
      ::json_object*	object;

      enter();

      // XXX check if value is NULL

      // check the type.
      if (::json_object_is_type(this->value, ::json_type_object) == 0)
	false();

      // check if the key is valid.
      object = ::json_object_object_get(this->value, key.c_str());

      // check the error.
      if (is_error(object))
	{
      // XXX
      printf("-> false\n");

	false();
	}

      // XXX
      printf("-> true\n");

      true();
    }

    ///
    /// this operator compares two objects.
    ///
    Boolean		JSON::Node::operator==(
                          const JSON::Node&			element) const
    {
      enter();

      /* XXX todo
      // compare the values.
      if (*this->value != *element.value)
	false();
      */

      true();
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(JSON::Node, _());

    ///
    /// this function dumps the object.
    ///
    Status		JSON::Node::Dump(Natural32		margin) const
    {
      String		alignment(margin, ' ');
 
      enter();

      // is the value null?
      if (this->value == NULL)
	{
	  // dump the value.
	  std::cout << alignment << "[Node(null) "
		    << std::hex << this->value
		    << "] "
		    << none << std::endl;
	}
      else
	{
	  // depending on the value's type.
	  switch (::json_object_get_type(this->value))
	    {
	    case ::json_type_boolean:
	      {
		// dump the value.
		std::cout << alignment << "[Node(boolean) "
			  << std::hex << this->value
			  << "] "
			  << ::json_object_get_boolean(this->value)
			  << std::endl;

		break;
	      }
	    case ::json_type_double:
	      {
		// dump the value.
		std::cout << alignment << "[Node(real) "
			  << std::hex << this->value
			  << "] "
			  << ::json_object_get_double(this->value)
			  << std::endl;

		break;
	      }
	    case ::json_type_int:
	      {
		// dump the value.
		std::cout << alignment << "[Node(integer) "
			  << std::hex << this->value
			  << "] "
			  << std::dec
			  << ::json_object_get_int(this->value)
			  << std::endl;

		break;
	      }
	    case ::json_type_object:
	      {
		// dump the value.
		std::cout << alignment << "[Node(object) "
			  << std::hex << this->value
			  << "]" << std::endl;

		// go through the members.
		json_object_object_foreach(this->value, key, value)
		  {
		    Node	node(value);

		    // dump the key.
		    std::cout << alignment << Dumpable::Shift
			      << "[Key] " << String(key) << std::endl;

		    // dump the node.
		    if (node.Dump(margin + 4) == StatusError)
		      escape("unable to dump the node");
		  }

		break;
	      }
	    case ::json_type_array:
	      {
		Natural32	i;

		// dump the value.
		std::cout << alignment << "[Node(array) "
			  << std::hex << this->value
			  << "]"
			  << std::endl;

		// go through the elements.
		for(i = 0;
		    i < static_cast<Natural32>(
		      ::json_object_array_length(this->value));
		    i++)
		  {
		    Node	node(::json_object_array_get_idx(this->value,
								 i));

		    // dump the node.
		    if (node.Dump(margin + 2) == StatusError)
		      escape("unable to dump the node");
		  }

		break;
	      }
	    case ::json_type_string:
	      {
		// dump the value.
		std::cout << alignment << "[Node(string)] "
			  << String(::json_object_get_string(this->value))
			  << std::endl;

		break;
	      }
	    default:
	      {
		escape("unknown or unhandled type '%u'",
		       ::json_object_get_type(this->value));
	      }
	    }
	}

      leave();
    }

//
// ---------- document --------------------------------------------------------
//

    ///
    /// this method appends a bulk of nodes to the root node.
    ///
    Status		JSON::Document::Append(const Bulk&	bulk)
    {
      enter();

      // append the bulk to the root node.
      if (this->root.Append(bulk) == StatusError)
	escape("unable to append the root node");

      leave();
    }

    ///
    /// this method clears the elements from root node.
    ///
    Status		JSON::Document::Clear()
    {
      enter();

      // clear the root node.
      if (this->root.Clear() == StatusError)
	escape("unable to clear the root node");

      leave();
    }

    ///
    /// this method returns the number of elements in the root array.
    ///
    Status		JSON::Document::Size(Natural32&		size)
    {
      enter();

      // size the root node.
      if (this->root.Size(size) == StatusError)
	escape("unable to size the root node");

      leave();
    }

    ///
    /// this operator compares two objects.
    ///
    Boolean		JSON::Document::operator==(
                          const JSON::Document&			element) const
    {
      enter();

      // compare the root node.
      if (this->root != element.root)
	false();

      true();
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(JSON::Document, _());

    ///
    /// this function dumps the object.
    ///
    Status		JSON::Document::Dump(Natural32		margin) const
    {
      String		alignment(margin, ' ');
 
      enter();

      std::cout << alignment << "[Document]" << std::endl;

      // dump the root node.
      if (this->root.Dump(margin + 2) == StatusError)
	escape("unable to dump the root node");

      leave();
    }

//
// ---------- json ------------------------------------------------------------
//

    ///
    /// this method takes a valid JSON document and turns it into a
    /// string representation.
    ///
    Status		JSON::Encode(const Document&		document,
				     String&			string)
    {
      enter();

      // transform the JSON document into a JSON string.
      string.assign(::json_object_to_json_string(document.root.value));

      leave();
    }

    ///
    /// this method decodes a string-based JSON representation into a
    /// valid document.
    ///
    Status		JSON::Decode(const String&		string,
				     Document&			document)
    {
      enter();

      // parse the JSON string.
      document.root.value = json_tokener_parse(string.c_str());

      // check the error.
      if (is_error(document.root.value))
	escape("unable to parse the JSON string");

      leave();
    }

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{

  ///
  /// this method streams a JSON document.
  ///
  ostream&		operator<<(
                          ostream&				stream,
			  const elle::utility::JSON::Document&	element)
  {
    // stream the document's root node.
    stream << element.root;

    return (stream);
  }

  ///
  /// this method streams a JSON node.
  ///
  ostream&		operator<<(
                          ostream&				stream,
			  const elle::utility::JSON::Node&	element)
  {
    // stream the node's value.
    stream << elle::core::String(::json_object_to_json_string(element.value));

    return (stream);
  }

}
