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
// ---------- node ------------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		JSON::Node::Create(const ::Json::Value&	value)
    {
      enter();

      // set the value.
      this->value = value;

      leave();
    }

    ///
    /// XXX
    ///
    Status		JSON::Node::Type(enum JSON::Node::Type&	type)
    {
      enter();

      // return the type.
      type = static_cast<enum JSON::Node::Type>(this->value.type());

      leave();
    }

    ///
    /// XXX
    ///
    Status		JSON::Node::Get(Node&			node) const
    {
      enter();

      // return the current node.
      node = *this;

      leave();
    }

    ///
    /// XXX
    ///
    Status		JSON::Node::Get(Null&) const
    {
      enter();

      // check the type.
      if (this->value.type() != ::Json::nullValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value.type(), ::Json::nullValue);

      leave();
    }

    ///
    /// XXX
    ///
    Status		JSON::Node::Get(Integer8&		element) const
    {
      enter();

      // check the type.
      if (this->value.type() != ::Json::intValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value.type(), ::Json::intValue);

      // return the value.
      element = this->value.asInt();

      leave();
    }

    ///
    /// XXX
    ///
    Status		JSON::Node::Get(Integer16&		element) const
    {
      enter();

      // check the type.
      if (this->value.type() != ::Json::intValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value.type(), ::Json::intValue);

      // return the value.
      element = this->value.asInt();

      leave();
    }

    ///
    /// XXX
    ///
    Status		JSON::Node::Get(Integer32&		element) const
    {
      enter();

      // check the type.
      if (this->value.type() != ::Json::intValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value.type(), ::Json::intValue);

      // return the value.
      element = this->value.asInt();

      leave();
    }

    ///
    /// XXX
    ///
    Status		JSON::Node::Get(Integer64&		element) const
    {
      enter();

      // check the type.
      if (this->value.type() != ::Json::intValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value.type(), ::Json::intValue);

      // return the value.
      element = this->value.asInt();

      leave();
    }

    ///
    /// XXX
    ///
    Status		JSON::Node::Get(Natural8&		element) const
    {
      enter();

      // check the type.
      if (this->value.type() != ::Json::uintValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value.type(), ::Json::uintValue);

      // return the value.
      element = this->value.asUInt();

      leave();
    }

    ///
    /// XXX
    ///
    Status		JSON::Node::Get(Natural16&		element) const
    {
      enter();

      // check the type.
      if (this->value.type() != ::Json::uintValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value.type(), ::Json::uintValue);

      // return the value.
      element = this->value.asUInt();

      leave();
    }

    ///
    /// XXX
    ///
    Status		JSON::Node::Get(Natural32&		element) const
    {
      enter();

      // check the type.
      if (this->value.type() != ::Json::uintValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value.type(), ::Json::uintValue);

      // return the value.
      element = this->value.asUInt();

      leave();
    }

    ///
    /// XXX
    ///
    Status		JSON::Node::Get(Natural64&		element) const
    {
      enter();

      // check the type.
      if (this->value.type() != ::Json::uintValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value.type(), ::Json::uintValue);

      // return the value.
      element = this->value.asUInt();

      leave();
    }

    ///
    /// XXX
    ///
    Status		JSON::Node::Get(Real&			element) const
    {
      enter();

      // check the type.
      if (this->value.type() != ::Json::realValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value.type(), ::Json::realValue);

      // return the value.
      element = this->value.asDouble();

      leave();
    }

    ///
    /// XXX
    ///
    Status		JSON::Node::Get(String&			element) const
    {
      enter();

      // check the type.
      if (this->value.type() != ::Json::stringValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value.type(), ::Json::stringValue);

      // return the value.
      element = this->value.asString();

      leave();
    }

    ///
    /// XXX
    ///
    Status		JSON::Node::Get(Boolean&		element) const
    {
      enter();

      // check the type.
      if (this->value.type() != ::Json::booleanValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value.type(), ::Json::booleanValue);

      // return the value.
      element = this->value.asBool();

      leave();
    }

    ///
    /// XXX
    ///
    Status		JSON::Node::Get(const Natural32&	index,
					Node&			node) const
    {
      ::Json::Value	value;

      enter();

      // check the type.
      if (this->value.type() != ::Json::arrayValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value.type(), ::Json::arrayValue);

      // check if the index is valid.
      if (this->value.isValidIndex(index) == false)
	escape("the given index '%u' seems to be out-of-bound given the "
	       "size of the array '%u'",
	       index,
	       this->value.size());

      // retrieve the value.
      value = this->value[index];

      // create the node.
      if (node.Create(value) == StatusError)
	escape("unable to create the node");

      leave();
    }

    ///
    /// XXX
    ///
    Status		JSON::Node::Get(const String&		key,
					Node&			node) const
    {
      ::Json::Value	value;

      enter();

      // check the type.
      if (this->value.type() != ::Json::objectValue)
	escape("invalid type: node's(%u) expected(%u)",
	       this->value.type(), ::Json::objectValue);

      // check if the key is valid.
      if (this->value.isMember(key) == false)
	escape("the given key '%s' seems to be invalid",
	       key.c_str());

      // retrieve the value.
      value = this->value[key];

      // create the node.
      if (node.Create(value) == StatusError)
	escape("unable to create the node");

      leave();
    }

    ///
    /// XXX
    ///
    Status		JSON::Node::Get(Uniquable&		uniquable)
      const
    {
      Unique		unique;

      enter();

      // retrieve a string.
      if (this->Get(unique) == StatusError)
	escape("unable to retrieve the unique representation");

      // restore the uniquable.
      if (uniquable.Restore(unique) == StatusError)
	escape("unable to restore the uniquable");

      leave();
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
      switch (this->value.type())
	{
	case ::Json::nullValue:
	  {
	    // dump the value.
	    std::cout << alignment << "[Node(nul)] "
		      << none << std::endl;

	    break;
	  }
	case ::Json::intValue:
	  {
	    // dump the value.
	    std::cout << alignment << "[Node(int)] "
		      << std::dec << this->value.asInt() << std::endl;

	    break;
	  }
	case ::Json::uintValue:
	  {
	    // dump the value.
	    std::cout << alignment << "[Node(uint)] "
		      << std::dec << this->value.asUInt() << std::endl;

	    break;
	  }
	case ::Json::realValue:
	  {
	    // dump the value.
	    std::cout << alignment << "[Node(real)] "
		      << this->value.asDouble() << std::endl;

	    break;
	  }
	case ::Json::stringValue:
	  {
	    // dump the value.
	    std::cout << alignment << "[Node(string)] "
		      << this->value.asString() << std::endl;

	    break;
	  }
	case ::Json::booleanValue:
	  {
	    // dump the value.
	    std::cout << alignment << "[Node(boolean)] "
		      << this->value.asBool() << std::endl;

	    break;
	  }
	case ::Json::arrayValue:
	  {
	    Natural32		i;

	    // dump the value.
	    std::cout << alignment << "[Node(array)]"
		      << std::endl;

	    // go through the elements.
	    for (i = 0; i < this->value.size(); i++)
	      {
		Node		node;

		// create the node.
		if (node.Create(this->value[i]) == StatusError)
		  escape("unable to create the node");

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
	    members = this->value.getMemberNames();

	    // go through the members.
	    for (i = 0; i < members.size(); i++)
	      {
		Node			node;

		// dump the key.
		std::cout << alignment << Dumpable::Shift
			  << "[Key] " << members[i] << std::endl;

		// create the node.
		if (node.Create(this->value[members[i]]) == StatusError)
		  escape("unable to create the node");

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
    /// XXX
    ///
    Status		JSON::Document::Load(const String&	content)
    {
      ::Json::Reader	reader;
      ::Json::Value	value;

      enter();

      // parse the content.
      if (reader.parse(content, value) == false)
	escape(reader.getFormatedErrorMessages().c_str());

      // create the root node.
      if (this->root.Create(value) == StatusError)
	escape("unable to create the root node");

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
    stream << element.value;

    return (stream);
  }

}
