//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien.quintard   [tue nov 22 12:50:06 2011]
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
// ---------- json ------------------------------------------------------------
//

    ///
    /// this method takes a valid JSON document and turns it into a
    /// string representation.
    ///
    Status		JSON::Encode(const Document&		document,
				     String&			string)
    {
      std::stringstream	stream;

      enter();

      // transform the request into a string.
      stream << document;

      // return the string-based JSON document.
      string = stream.str();

      leave();
    }

    ///
    /// this method decodes a string-based JSON representation into a
    /// valid document.
    ///
    Status		JSON::Decode(const String&		string,
				     Document&			document)
    {
      ::Json::Reader	reader;

      enter();

      // parse the content.
      if (reader.parse(string, *document.root.value) == false)
	escape(reader.getFormatedErrorMessages().c_str());

      leave();
    }

//
// ---------- node ------------------------------------------------------------
//

    ///
    /// the default constructor sets the node so as to receive data
    /// by allocating a value.
    ///
    JSON::Node::Node():
      mode(ModeAllocated),
      value(new ::Json::Value)
    {
    }

    ///
    /// this constructor wraps the given value.
    ///
    JSON::Node::Node(::Json::Value*				value):
      mode(ModeWrapped),
      value(value)
    {
    }

    ///
    /// the copy constructor.
    ///
    JSON::Node::Node(const Node&				node):
      Object(node),

      mode(node.mode)
    {
      // depending on the mode.
      switch (node.mode)
	{
	case JSON::Node::ModeAllocated:
	  {
	    // duplicate the value.
	    this->value = new Json::Value(*node.value);

	    break;
	  }
	case JSON::Node::ModeWrapped:
	  {
	    // set the value pointer.
	    this->value = node.value;

	    break;
	  }
	}
    }

    ///
    /// the destructor.
    ///
    JSON::Node::~Node()
    {
      // depending on the mode.
      switch (this->mode)
	{
	case JSON::Node::ModeAllocated:
	  {
	    // delete the value.
	    delete this->value;

	    break;
	  }
	case JSON::Node::ModeWrapped:
	  {
	    // nothing to do.

	    break;
	  }
	}
    }

    ///
    /// this method returns the type of the node.
    ///
    Status		JSON::Node::Type(enum JSON::Node::Type&	type)
    {
      enter();

      // return the type.
      type = static_cast<enum JSON::Node::Type>(this->value->type());

      leave();
    }

    ///
    /// this method wraps the given value within the node.
    ///
    Status		JSON::Node::Wrap(::Json::Value*		value)
    {
      JSON::Node	node(value);

      enter();

      // recycle the object.
      if (this->Recycle(&node) == StatusError)
	escape("unable to recycle the object");

      leave();
    }

    ///
    /// this method returns a null value.
    ///
    Status		JSON::Node::Get(Null&) const
    {
      enter();

      // check the type.
      if (this->value->type() != ::Json::nullValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value->type(), ::Json::nullValue);

      leave();
    }

    ///
    /// this method returns a boolean value.
    ///
    Status		JSON::Node::Get(Boolean&		element) const
    {
      enter();

      // check the type.
      if (this->value->type() != ::Json::booleanValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value->type(), ::Json::booleanValue);

      // return the value.
      element = this->value->asBool();

      leave();
    }

    ///
    /// this method returns a integer value.
    ///
    Status		JSON::Node::Get(Integer8&		element) const
    {
      enter();

      // check the type.
      if (this->value->type() != ::Json::intValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value->type(), ::Json::intValue);

      // return the value.
      element = this->value->asInt();

      leave();
    }

    ///
    /// this method returns a integer value.
    ///
    Status		JSON::Node::Get(Integer16&		element) const
    {
      enter();

      // check the type.
      if (this->value->type() != ::Json::intValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value->type(), ::Json::intValue);

      // return the value.
      element = this->value->asInt();

      leave();
    }

    ///
    /// this method returns a integer value.
    ///
    Status		JSON::Node::Get(Integer32&		element) const
    {
      enter();

      // check the type.
      if (this->value->type() != ::Json::intValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value->type(), ::Json::intValue);

      // return the value.
      element = this->value->asInt();

      leave();
    }

    ///
    /// this method returns a integer value.
    ///
    Status		JSON::Node::Get(Integer64&		element) const
    {
      enter();

      // check the type.
      if (this->value->type() != ::Json::intValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value->type(), ::Json::intValue);

      // return the value.
      element = this->value->asInt();

      leave();
    }

    ///
    /// this method returns a natural value.
    ///
    Status		JSON::Node::Get(Natural8&		element) const
    {
      enter();

      // check the type.
      if (this->value->type() != ::Json::uintValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value->type(), ::Json::uintValue);

      // return the value.
      element = this->value->asUInt();

      leave();
    }

    ///
    /// this method returns a natural value.
    ///
    Status		JSON::Node::Get(Natural16&		element) const
    {
      enter();

      // check the type.
      if (this->value->type() != ::Json::uintValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value->type(), ::Json::uintValue);

      // return the value.
      element = this->value->asUInt();

      leave();
    }

    ///
    /// this method returns a natural value.
    ///
    Status		JSON::Node::Get(Natural32&		element) const
    {
      enter();

      // check the type.
      if (this->value->type() != ::Json::uintValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value->type(), ::Json::uintValue);

      // return the value.
      element = this->value->asUInt();

      leave();
    }

    ///
    /// this method returns a natural value.
    ///
    Status		JSON::Node::Get(Natural64&		element) const
    {
      enter();

      // check the type.
      if (this->value->type() != ::Json::uintValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value->type(), ::Json::uintValue);

      // return the value.
      element = this->value->asUInt();

      leave();
    }

    ///
    /// this method returns a real value.
    ///
    Status		JSON::Node::Get(Real&			element) const
    {
      enter();

      // check the type.
      if (this->value->type() != ::Json::realValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value->type(), ::Json::realValue);

      // return the value.
      element = this->value->asDouble();

      leave();
    }

    ///
    /// this method returns a string value.
    ///
    Status		JSON::Node::Get(String&			element) const
    {
      enter();

      // check the type.
      if (this->value->type() != ::Json::stringValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value->type(), ::Json::stringValue);

      // return the value.
      element = this->value->asString();

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
      if (this->value->type() != ::Json::arrayValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value->type(), ::Json::arrayValue);

      // check if the index is valid.
      if (this->value->isValidIndex(index) == false)
	escape("the given index '%u' seems to be out-of-bound given the "
	       "size of the array '%u'",
	       index,
	       this->value->size());

      // retrieve the value and wrap it in the node.
      if (node.Wrap(&(*this->value)[index]) == StatusError)
	escape("unable to wrap the value");

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
      if (this->value->type() != ::Json::objectValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value->type(), ::Json::objectValue);

      // check if the key is valid.
      if (this->value->isMember(key) == false)
	escape("the given key '%s' seems to be invalid",
	       key.c_str());

      // retrieve the value and wrap it in the node.
      if (node.Wrap(&(*this->value)[key]) == StatusError)
	escape("unable to wrap the value");

      leave();
    }

    ///
    /// this method returns the current node.
    ///
    Status		JSON::Node::Get(Node&			node) const
    {
      enter();

      // wrap the current value.
      if (node.Wrap(this->value) == StatusError)
	escape("unable to wrap the value");

      leave();
    }

    ///
    /// this method records a null value.
    ///
    Status		JSON::Node::Set(const Null&)
    {
      enter();

      // set the value.
      *this->value = ::Json::Value(::Json::nullValue);

      leave();
    }

    ///
    /// this method records a boolean value.
    ///
    Status		JSON::Node::Set(const Boolean&		element)
    {
      enter();

      // set the value.
      *this->value = ::Json::Value(element);

      leave();
    }

    ///
    /// this method records a integer value.
    ///
    Status		JSON::Node::Set(const Integer8&		element)
    {
      enter();

      // set the value.
      *this->value = ::Json::Value(element);

      leave();
    }

    ///
    /// this method records a integer value.
    ///
    Status		JSON::Node::Set(const Integer16&	element)
    {
      enter();

      // set the value.
      *this->value = ::Json::Value(element);

      leave();
    }

    ///
    /// this method records a integer value.
    ///
    Status		JSON::Node::Set(const Integer32&	element)
    {
      enter();

      // set the value.
      *this->value = ::Json::Value(element);

      leave();
    }

    ///
    /// this method records a integer value.
    ///
    Status		JSON::Node::Set(const Integer64&	element)
    {
      enter();

      // set the value.
      *this->value = ::Json::Value(static_cast< ::Json::Int >(element));

      leave();
    }

    ///
    /// this method records a natural value.
    ///
    Status		JSON::Node::Set(const Natural8&		element)
    {
      enter();

      // set the value.
      *this->value = ::Json::Value(element);

      leave();
    }

    ///
    /// this method records a natural value.
    ///
    Status		JSON::Node::Set(const Natural16&	element)
    {
      enter();

      // set the value.
      *this->value = ::Json::Value(element);

      leave();
    }

    ///
    /// this method records a natural value.
    ///
    Status		JSON::Node::Set(const Natural32&	element)
    {
      enter();

      // set the value.
      *this->value = ::Json::Value(element);

      leave();
    }

    ///
    /// this method records a natural value.
    ///
    Status		JSON::Node::Set(const Natural64&	element)
    {
      enter();

      // set the value.
      *this->value = ::Json::Value(static_cast< ::Json::UInt >(element));

      leave();
    }

    ///
    /// this method records a real value.
    ///
    Status		JSON::Node::Set(const Real&		element)
    {
      enter();

      // set the value.
      *this->value = ::Json::Value(element);

      leave();
    }

    ///
    /// this method records a string value.
    ///
    Status		JSON::Node::Set(const String&		element)
    {
      enter();

      // set the value.
      *this->value = ::Json::Value(element);

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
      if (this->value->type() != ::Json::arrayValue)
	{
	  // reset the value.
	  *this->value = ::Json::Value(::Json::nullValue);
	}

      // set the value.
      (*this->value)[index] = *node.value;

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
      if (this->value->type() != ::Json::objectValue)
	{
	  // reset the value.
	  *this->value = ::Json::Value(::Json::nullValue);
	}

      // set the value.
      (*this->value)[key] = *node.value;

      leave();
    }

    ///
    /// this method records a node.
    ///
    Status		JSON::Node::Set(const Node&		node)
    {
      enter();

      // set the value.
      *this->value = node.value;

      leave();
    }

    ///
    /// this method appends a node to the array.
    ///
    Status		JSON::Node::Append(const Node&		node)
    {
      enter();

      // check the type.
      if (this->value->type() != ::Json::arrayValue)
	{
	  // reset the value.
	  *this->value = ::Json::Value(::Json::nullValue);
	}

      // append the node's value.
      this->value->append(*node.value);

      leave();
    }

    ///
    /// this method appends nodes to the array.
    ///
    Status		JSON::Node::Append(const Node&		node1,
					   const Node&		node2)
    {
      enter();

      // append the nodes.
      if (this->Append(node1) == StatusError)
	escape("unable to append the nodes");

      // append the final node.
      if (this->Append(node2) == StatusError)
	escape("unable to append the nodes");

      leave();
    }


    ///
    /// this method appends nodes to the array.
    ///
    Status		JSON::Node::Append(const Node&		node1,
					   const Node&		node2,
					   const Node&		node3)
    {
      enter();

      // append the nodes.
      if (this->Append(node1, node2) == StatusError)
	escape("unable to append the nodes");

      // append the final node.
      if (this->Append(node3) == StatusError)
	escape("unable to append the nodes");

      leave();
    }

    ///
    /// this method appends nodes to the array.
    ///
    Status		JSON::Node::Append(const Node&		node1,
					   const Node&		node2,
					   const Node&		node3,
					   const Node&		node4)
    {
      enter();

      // append the nodes.
      if (this->Append(node1, node2, node3) == StatusError)
	escape("unable to append the nodes");

      // append the final node.
      if (this->Append(node4) == StatusError)
	escape("unable to append the nodes");

      leave();
    }

    ///
    /// this method appends nodes to the array.
    ///
    Status		JSON::Node::Append(const Node&		node1,
					   const Node&		node2,
					   const Node&		node3,
					   const Node&		node4,
					   const Node&		node5)
    {
      enter();

      // append the nodes.
      if (this->Append(node1, node2, node3, node4) == StatusError)
	escape("unable to append the nodes");

      // append the final node.
      if (this->Append(node5) == StatusError)
	escape("unable to append the nodes");

      leave();
    }

    ///
    /// this method appends nodes to the array.
    ///
    Status		JSON::Node::Append(const Node&		node1,
					   const Node&		node2,
					   const Node&		node3,
					   const Node&		node4,
					   const Node&		node5,
					   const Node&		node6)
    {
      enter();

      // append the nodes.
      if (this->Append(node1, node2, node3, node4, node5) == StatusError)
	escape("unable to append the nodes");

      // append the final node.
      if (this->Append(node6) == StatusError)
	escape("unable to append the nodes");

      leave();
    }

    ///
    /// this method appends nodes to the array.
    ///
    Status		JSON::Node::Append(const Node&		node1,
					   const Node&		node2,
					   const Node&		node3,
					   const Node&		node4,
					   const Node&		node5,
					   const Node&		node6,
					   const Node&		node7)
    {
      enter();

      // append the nodes.
      if (this->Append(node1, node2, node3, node4, node5,
		       node6) == StatusError)
	escape("unable to append the nodes");

      // append the final node.
      if (this->Append(node7) == StatusError)
	escape("unable to append the nodes");

      leave();
    }

    ///
    /// this method appends nodes to the array.
    ///
    Status		JSON::Node::Append(const Node&		node1,
					   const Node&		node2,
					   const Node&		node3,
					   const Node&		node4,
					   const Node&		node5,
					   const Node&		node6,
					   const Node&		node7,
					   const Node&		node8)
    {
      enter();

      // append the nodes.
      if (this->Append(node1, node2, node3, node4, node5,
		       node6, node7) == StatusError)
	escape("unable to append the nodes");

      // append the final node.
      if (this->Append(node8) == StatusError)
	escape("unable to append the nodes");

      leave();
    }

    ///
    /// this method appends nodes to the array.
    ///
    Status		JSON::Node::Append(const Node&		node1,
					   const Node&		node2,
					   const Node&		node3,
					   const Node&		node4,
					   const Node&		node5,
					   const Node&		node6,
					   const Node&		node7,
					   const Node&		node8,
					   const Node&		node9)
    {
      enter();

      // append the nodes.
      if (this->Append(node1, node2, node3, node4, node5,
		       node6, node7, node8) == StatusError)
	escape("unable to append the nodes");

      // append the final node.
      if (this->Append(node9) == StatusError)
	escape("unable to append the nodes");

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
      if (this->value->type() != ::Json::arrayValue)
	false();

      // check if the index is valid.
      if (this->value->isValidIndex(index) == false)
	false();

      true();
    }

    ///
    /// this method returns true if the given key is valid within the
    /// object.
    ///
    Status		JSON::Node::Exist(const String&		key)
    {
      enter();

      // check the type.
      if (this->value->type() != ::Json::objectValue)
	false();

      // check if the key is valid.
      if (this->value->isMember(key) == false)
	false();

      true();
    }

    ///
    /// this operator compares two objects.
    ///
    Boolean		JSON::Node::operator==(
                          const JSON::Node&			element) const
    {
      enter();

      // compare the values.
      if (*this->value != *element.value)
	false();

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

      // depending on the value's type.
      switch (this->value->type())
	{
	case ::Json::nullValue:
	  {
	    // dump the value.
	    std::cout << alignment << "[Node(null)] "
		      << none << std::endl;

	    break;
	  }
	case ::Json::intValue:
	  {
	    // dump the value.
	    std::cout << alignment << "[Node(int)] "
		      << std::dec << this->value->asInt() << std::endl;

	    break;
	  }
	case ::Json::uintValue:
	  {
	    // dump the value.
	    std::cout << alignment << "[Node(uint)] "
		      << std::dec << this->value->asUInt() << std::endl;

	    break;
	  }
	case ::Json::realValue:
	  {
	    // dump the value.
	    std::cout << alignment << "[Node(real)] "
		      << this->value->asDouble() << std::endl;

	    break;
	  }
	case ::Json::stringValue:
	  {
	    // dump the value.
	    std::cout << alignment << "[Node(string)] "
		      << this->value->asString() << std::endl;

	    break;
	  }
	case ::Json::booleanValue:
	  {
	    // dump the value.
	    std::cout << alignment << "[Node(boolean)] "
		      << this->value->asBool() << std::endl;

	    break;
	  }
	case ::Json::arrayValue:
	  {
	    Natural32		i;

	    // dump the value.
	    std::cout << alignment << "[Node(array)]"
		      << std::endl;

	    // go through the elements.
	    for (i = 0; i < this->value->size(); i++)
	      {
		Node		node(&(*this->value)[i]);

		// dump the node.
		if (node.Dump(margin + 2) == StatusError)
		  escape("unable to dump the node");
	      }

	    break;
	  }
	case ::Json::objectValue:
	  {
	    ::Json::Value::Members	members;
	    Natural32			i;

	    // dump the value.
	    std::cout << alignment << "[Node(object)]" << std::endl;

	    // retrieve the object's members.
	    members = this->value->getMemberNames();

	    // go through the members.
	    for (i = 0; i < members.size(); i++)
	      {
		Node			node(&(*this->value)[members[i]]);

		// dump the key.
		std::cout << alignment << Dumpable::Shift
			  << "[Key] " << members[i] << std::endl;

		// dump the node.
		if (node.Dump(margin + 4) == StatusError)
		  escape("unable to dump the node");
	      }

	    break;
	  }
	}

      leave();
    }

//
// ---------- document --------------------------------------------------------
//

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
    stream << *element.value;

    return (stream);
  }

}
