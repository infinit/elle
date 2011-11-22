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
