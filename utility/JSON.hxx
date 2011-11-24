//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien.quintard   [wed nov 23 01:38:00 2011]
//

#ifndef ELLE_UTILITY_JSON_HXX
#define ELLE_UTILITY_JSON_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  using namespace standalone;

  namespace utility
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this constructor makes it easy to initialize a node with a given
    /// value.
    ///
    template <typename T>
    JSON::Node::Node(const T&					element):
      mode(ModeAllocated),
      value(new ::Json::Value)
    {
      enter();

      // set a default value.
      if (this->Set(element) == elle::StatusError)
	yield(_(), "unable to set a default value");

      release();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// these methods operate on Uniquable<F>, hence making it extremely
    /// simple to encode/decode Elle objects within JSON strings.
    ///

    template <Format F>
    Status		JSON::Node::Get(Uniquable<F>&		uniquable)
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

    template <Format F>
    Status		JSON::Node::Set(const Uniquable<F>&	uniquable)
    {
      Unique		unique;

      enter();

      // save the uniquable.
      if (uniquable.Save(unique) == StatusError)
	escape("unable to save the uniquable");

      // set the string.
      if (this->Set(unique) == StatusError)
	escape("unable to set the unique representation");

      leave();
    }

    ///
    /// these methods provide an easy way for getting and setting values
    /// from/to the document, given a path, hence anywhere in the document.
    ///
    /// in addition, the Dig() methods pave the way to the given target,
    /// creating the node in the document if necessary.
    ///
    /// finally, the Append() methods append a bulk of nodes to the path-ed
    /// element while Erase() erases a path-ed element.
    ///

    template <typename X>
    Status		JSON::Document::Get(X&			target) const
    {
      enter();

      // retrieve the target value for the root node.
      if (this->root.Get(target) == StatusError)
	escape("unable to retrieve the target value");

      leave();
    }

    template <typename T1,
	      typename X>
    Status		JSON::Document::Get(const T1&		id1,
					    X&			target) const
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->root.Get(id1, node) == StatusError)
	escape("unable to retrieve the node associated with the "
	       "given identifiers");

      // finally, retrieve the target value for this node.
      if (node.Get(target) == StatusError)
	escape("unable to retrieve the target value");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename X>
    Status		JSON::Document::Get(const T1&		id1,
					    const T2&		id2,
					    X&			target) const
    {
      Node		base;
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1,
		    base) == StatusError)
	escape("unable to retrieve the node associated with the "
	       "given identifiers");

      // retrieve the node given the identifiers.
      if (base.Get(id2, node) == StatusError)
	escape("unable to retrieve the node associated with the "
	       "given identifiers");

      // finally, retrieve the target value for this node.
      if (node.Get(target) == StatusError)
	escape("unable to retrieve the target value");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename X>
    Status		JSON::Document::Get(const T1&		id1,
					    const T2&		id2,
					    const T3&		id3,
					    X&			target) const
    {
      Node		base;
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2,
		    base) == StatusError)
	escape("unable to retrieve the node associated with the "
	       "given identifiers");

      // retrieve the node given the identifiers.
      if (base.Get(id3, node) == StatusError)
	escape("unable to retrieve the node associated with the "
	       "given identifiers");

      // finally, retrieve the target value for this node.
      if (node.Get(target) == StatusError)
	escape("unable to retrieve the target value");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename X>
    Status		JSON::Document::Get(const T1&		id1,
					    const T2&		id2,
					    const T3&		id3,
					    const T4&		id4,
					    X&			target) const
    {
      Node		base;
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2, id3,
		    base) == StatusError)
	escape("unable to retrieve the node associated with the "
	       "given identifiers");

      // retrieve the node given the identifiers.
      if (base.Get(id4, node) == StatusError)
	escape("unable to retrieve the node associated with the "
	       "given identifiers");

      // finally, retrieve the target value for this node.
      if (node.Get(target) == StatusError)
	escape("unable to retrieve the target value");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename X>
    Status		JSON::Document::Get(const T1&		id1,
					    const T2&		id2,
					    const T3&		id3,
					    const T4&		id4,
					    const T5&		id5,
					    X&			target) const
    {
      Node		base;
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2, id3, id4,
		    base) == StatusError)
	escape("unable to retrieve the node associated with the "
	       "given identifiers");

      // retrieve the node given the identifiers.
      if (base.Get(id5, node) == StatusError)
	escape("unable to retrieve the node associated with the "
	       "given identifiers");

      // finally, retrieve the target value for this node.
      if (node.Get(target) == StatusError)
	escape("unable to retrieve the target value");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename X>
    Status		JSON::Document::Get(const T1&		id1,
					    const T2&		id2,
					    const T3&		id3,
					    const T4&		id4,
					    const T5&		id5,
					    const T6&		id6,
					    X&			target) const
    {
      Node		base;
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2, id3, id4, id5,
		    base) == StatusError)
	escape("unable to retrieve the node associated with the "
	       "given identifiers");

      // retrieve the node given the identifiers.
      if (base.Get(id6, node) == StatusError)
	escape("unable to retrieve the node associated with the "
	       "given identifiers");

      // finally, retrieve the target value for this node.
      if (node.Get(target) == StatusError)
	escape("unable to retrieve the target value");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename X>
    Status		JSON::Document::Get(const T1&		id1,
					    const T2&		id2,
					    const T3&		id3,
					    const T4&		id4,
					    const T5&		id5,
					    const T6&		id6,
					    const T7&		id7,
					    X&			target) const
    {
      Node		base;
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2, id3, id4, id5, id6,
		    base) == StatusError)
	escape("unable to retrieve the node associated with the "
	       "given identifiers");

      // retrieve the node given the identifiers.
      if (base.Get(id7, node) == StatusError)
	escape("unable to retrieve the node associated with the "
	       "given identifiers");

      // finally, retrieve the target value for this node.
      if (node.Get(target) == StatusError)
	escape("unable to retrieve the target value");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8,
	      typename X>
    Status		JSON::Document::Get(const T1&		id1,
					    const T2&		id2,
					    const T3&		id3,
					    const T4&		id4,
					    const T5&		id5,
					    const T6&		id6,
					    const T7&		id7,
					    const T8&		id8,
					    X&			target) const
    {
      Node		base;
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2, id3, id4, id5, id6, id7,
		    base) == StatusError)
	escape("unable to retrieve the node associated with the "
	       "given identifiers");

      // retrieve the node given the identifiers.
      if (base.Get(id8, node) == StatusError)
	escape("unable to retrieve the node associated with the "
	       "given identifiers");

      // finally, retrieve the target value for this node.
      if (node.Get(target) == StatusError)
	escape("unable to retrieve the target value");

      leave();
    }

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
    Status		JSON::Document::Get(const T1&		id1,
					    const T2&		id2,
					    const T3&		id3,
					    const T4&		id4,
					    const T5&		id5,
					    const T6&		id6,
					    const T7&		id7,
					    const T8&		id8,
					    const T9&		id9,
					    X&			target) const
    {
      Node		base;
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2, id3, id4, id5, id6, id7, id8,
		    base) == StatusError)
	escape("unable to retrieve the node associated with the "
	       "given identifiers");

      // retrieve the node given the identifiers.
      if (base.Get(id9, node) == StatusError)
	escape("unable to retrieve the node associated with the "
	       "given identifiers");

      // finally, retrieve the target value for this node.
      if (node.Get(target) == StatusError)
	escape("unable to retrieve the target value");

      leave();
    }

    template <typename X>
    Status		JSON::Document::Set(const X&		target)
    {
      enter();

      // set the target value for the root node.
      if (this->root.Set(target) == StatusError)
	escape("unable to set the target value");

      leave();
    }

    template <typename T1,
	      typename X>
    Status		JSON::Document::Set(const T1&		id1,
					    const X&		target)
    {
      Node		node;

      enter();

      // dig the way to the target.
      if (this->Dig(id1,
		    node) == StatusError)
	escape("unable to dig the way to the target");

      // set the target value.
      if (node.Set(target) == StatusError)
	escape("unable to set the target value");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename X>
    Status		JSON::Document::Set(const T1&		id1,
					    const T2&		id2,
					    const X&		target)
    {
      Node		node;

      enter();

      // dig the way to the target.
      if (this->Dig(id1, id2,
		    node) == StatusError)
	escape("unable to dig the way to the target");

      // set the target value.
      if (node.Set(target) == StatusError)
	escape("unable to set the target value");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename X>
    Status		JSON::Document::Set(const T1&		id1,
					    const T2&		id2,
					    const T3&		id3,
					    const X&		target)
    {
      Node		node;

      enter();

      // dig the way to the target.
      if (this->Dig(id1, id2, id3,
		    node) == StatusError)
	escape("unable to dig the way to the target");

      // set the target value.
      if (node.Set(target) == StatusError)
	escape("unable to set the target value");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename X>
    Status		JSON::Document::Set(const T1&		id1,
					    const T2&		id2,
					    const T3&		id3,
					    const T4&		id4,
					    const X&		target)
    {
      Node		node;

      enter();

      // dig the way to the target.
      if (this->Dig(id1, id2, id3, id4,
		    node) == StatusError)
	escape("unable to dig the way to the target");

      // set the target value.
      if (node.Set(target) == StatusError)
	escape("unable to set the target value");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename X>
    Status		JSON::Document::Set(const T1&		id1,
					    const T2&		id2,
					    const T3&		id3,
					    const T4&		id4,
					    const T5&		id5,
					    const X&		target)
    {
      Node		node;

      enter();

      // dig the way to the target.
      if (this->Dig(id1, id2, id3, id4, id5,
		    node) == StatusError)
	escape("unable to dig the way to the target");

      // set the target value.
      if (node.Set(target) == StatusError)
	escape("unable to set the target value");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename X>
    Status		JSON::Document::Set(const T1&		id1,
					    const T2&		id2,
					    const T3&		id3,
					    const T4&		id4,
					    const T5&		id5,
					    const T6&		id6,
					    const X&		target)
    {
      Node		node;

      enter();

      // dig the way to the target.
      if (this->Dig(id1, id2, id3, id4, id5, id6,
		    node) == StatusError)
	escape("unable to dig the way to the target");

      // set the target value.
      if (node.Set(target) == StatusError)
	escape("unable to set the target value");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename X>
    Status		JSON::Document::Set(const T1&		id1,
					    const T2&		id2,
					    const T3&		id3,
					    const T4&		id4,
					    const T5&		id5,
					    const T6&		id6,
					    const T7&		id7,
					    const X&		target)
    {
      Node		node;

      enter();

      // dig the way to the target.
      if (this->Dig(id1, id2, id3, id4, id5, id6, id7,
		    node) == StatusError)
	escape("unable to dig the way to the target");

      // set the target value.
      if (node.Set(target) == StatusError)
	escape("unable to set the target value");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8,
	      typename X>
    Status		JSON::Document::Set(const T1&		id1,
					    const T2&		id2,
					    const T3&		id3,
					    const T4&		id4,
					    const T5&		id5,
					    const T6&		id6,
					    const T7&		id7,
					    const T8&		id8,
					    const X&		target)
    {
      Node		node;

      enter();

      // dig the way to the target.
      if (this->Dig(id1, id2, id3, id4, id5, id6, id7, id8,
		    node) == StatusError)
	escape("unable to dig the way to the target");

      // set the target value.
      if (node.Set(target) == StatusError)
	escape("unable to set the target value");

      leave();
    }

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
    Status		JSON::Document::Set(const T1&		id1,
					    const T2&		id2,
					    const T3&		id3,
					    const T4&		id4,
					    const T5&		id5,
					    const T6&		id6,
					    const T7&		id7,
					    const T8&		id8,
					    const T9&		id9,
					    const X&		target)
    {
      Node		node;

      enter();

      // dig the way to the target.
      if (this->Dig(id1, id2, id3, id4, id5, id6, id7, id8, id9,
		    node) == StatusError)
	escape("unable to dig the way to the target");

      // set the target value.
      if (node.Set(target) == StatusError)
	escape("unable to set the target value");

      leave();
    }

    template <typename T1>
    Status		JSON::Document::Dig(const T1&		id1,
					    Node&		node)
    {
      enter();

      // if the identifier does not exist...
      if (this->root.Exist(id1) == StatusFalse)
	{
	  // create the identified entry.
	  if (this->root.Set(id1, none) == StatusError)
	    escape("unable to create the identified entry");
	}

      // return the target node.
      if (this->root.Get(id1, node) == StatusError)
	escape("unable to retrieve the target node");

      leave();
    }

    template <typename T1,
	      typename T2>
    Status		JSON::Document::Dig(const T1&		id1,
					    const T2&		id2,
					    Node&		node)
    {
      Node		base;

      enter();

      // dig the root identifiers.
      if (this->Dig(id1,
		    base) == StatusError)
	escape("unable to dig the root identifiers");

      // if the identifier does not exist...
      if (base.Exist(id2) == StatusFalse)
	{
	  // create the identified entry.
	  if (base.Set(id2, none) == StatusError)
	    escape("unable to create the identified entry");
	}

      // return the target node.
      if (base.Get(id2, node) == StatusError)
	escape("unable to retrieve the target node");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3>
    Status		JSON::Document::Dig(const T1&		id1,
					    const T2&		id2,
					    const T3&		id3,
					    Node&		node)
    {
      Node		base;

      enter();

      // dig the root identifiers.
      if (this->Dig(id1, id2,
		    base) == StatusError)
	escape("unable to dig the root identifiers");

      // if the identifier does not exist...
      if (base.Exist(id3) == StatusFalse)
	{
	  // create the identified entry.
	  if (base.Set(id3, none) == StatusError)
	    escape("unable to create the identified entry");
	}

      // return the target node.
      if (base.Get(id3, node) == StatusError)
	escape("unable to retrieve the target node");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4>
    Status		JSON::Document::Dig(const T1&		id1,
					    const T2&		id2,
					    const T3&		id3,
					    const T4&		id4,
					    Node&		node)
    {
      Node		base;

      enter();

      // dig the root identifiers.
      if (this->Dig(id1, id2, id3,
		    base) == StatusError)
	escape("unable to dig the root identifiers");

      // if the identifier does not exist...
      if (base.Exist(id4) == StatusFalse)
	{
	  // create the identified entry.
	  if (base.Set(id4, none) == StatusError)
	    escape("unable to create the identified entry");
	}

      // return the target node.
      if (base.Get(id4, node) == StatusError)
	escape("unable to retrieve the target node");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5>
    Status		JSON::Document::Dig(const T1&		id1,
					    const T2&		id2,
					    const T3&		id3,
					    const T4&		id4,
					    const T5&		id5,
					    Node&		node)
    {
      Node		base;

      enter();

      // dig the root identifiers.
      if (this->Dig(id1, id2, id3, id4,
		    base) == StatusError)
	escape("unable to dig the root identifiers");

      // if the identifier does not exist...
      if (base.Exist(id5) == StatusFalse)
	{
	  // create the identified entry.
	  if (base.Set(id5, none) == StatusError)
	    escape("unable to create the identified entry");
	}

      // return the target node.
      if (base.Get(id5, node) == StatusError)
	escape("unable to retrieve the target node");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6>
    Status		JSON::Document::Dig(const T1&		id1,
					    const T2&		id2,
					    const T3&		id3,
					    const T4&		id4,
					    const T5&		id5,
					    const T6&		id6,
					    Node&		node)
    {
      Node		base;

      enter();

      // dig the root identifiers.
      if (this->Dig(id1, id2, id3, id4, id5,
		    base) == StatusError)
	escape("unable to dig the root identifiers");

      // if the identifier does not exist...
      if (base.Exist(id6) == StatusFalse)
	{
	  // create the identified entry.
	  if (base.Set(id6, none) == StatusError)
	    escape("unable to create the identified entry");
	}

      // return the target node.
      if (base.Get(id6, node) == StatusError)
	escape("unable to retrieve the target node");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7>
    Status		JSON::Document::Dig(const T1&		id1,
					    const T2&		id2,
					    const T3&		id3,
					    const T4&		id4,
					    const T5&		id5,
					    const T6&		id6,
					    const T7&		id7,
					    Node&		node)
    {
      Node		base;

      enter();

      // dig the root identifiers.
      if (this->Dig(id1, id2, id3, id4, id5, id6,
		    base) == StatusError)
	escape("unable to dig the root identifiers");

      // if the identifier does not exist...
      if (base.Exist(id7) == StatusFalse)
	{
	  // create the identified entry.
	  if (base.Set(id7, none) == StatusError)
	    escape("unable to create the identified entry");
	}

      // return the target node.
      if (base.Get(id7, node) == StatusError)
	escape("unable to retrieve the target node");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8>
    Status		JSON::Document::Dig(const T1&		id1,
					    const T2&		id2,
					    const T3&		id3,
					    const T4&		id4,
					    const T5&		id5,
					    const T6&		id6,
					    const T7&		id7,
					    const T8&		id8,
					    Node&		node)
    {
      Node		base;

      enter();

      // dig the root identifiers.
      if (this->Dig(id1, id2, id3, id4, id5, id6, id7,
		    base) == StatusError)
	escape("unable to dig the root identifiers");

      // if the identifier does not exist...
      if (base.Exist(id8) == StatusFalse)
	{
	  // create the identified entry.
	  if (base.Set(id8, none) == StatusError)
	    escape("unable to create the identified entry");
	}

      // return the target node.
      if (base.Get(id8, node) == StatusError)
	escape("unable to retrieve the target node");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8,
	      typename T9>
    Status		JSON::Document::Dig(const T1&		id1,
					    const T2&		id2,
					    const T3&		id3,
					    const T4&		id4,
					    const T5&		id5,
					    const T6&		id6,
					    const T7&		id7,
					    const T8&		id8,
					    const T9&		id9,
					    Node&		node)
    {
      Node		base;

      enter();

      // dig the root identifiers.
      if (this->Dig(id1, id2, id3, id4, id5, id6, id7, id8,
		    base) == StatusError)
	escape("unable to dig the root identifiers");

      // if the identifier does not exist...
      if (base.Exist(id9) == StatusFalse)
	{
	  // create the identified entry.
	  if (base.Set(id9, none) == StatusError)
	    escape("unable to create the identified entry");
	}

      // return the target node.
      if (base.Get(id9, node) == StatusError)
	escape("unable to retrieve the target node");

      leave();
    }

    template <typename T1>
    Status		JSON::Document::Append(const T1&	id1,
					       const Bulk&	bulk)
    {
      Node		node;

      enter();

      // dig the way to the target.
      if (this->Dig(id1,
		    node) == StatusError)
	escape("unable to dig the way to the target");

      // append the bulk to the node.
      if (node.Append(bulk) == StatusError)
	escape("unable to append the node");

      leave();
    }

    template <typename T1,
	      typename T2>
    Status		JSON::Document::Append(const T1&	id1,
					       const T2&	id2,
					       const Bulk&	bulk)
    {
      Node		node;

      enter();

      // dig the way to the target.
      if (this->Dig(id1, id2,
		    node) == StatusError)
	escape("unable to dig the way to the target");

      // append the bulk to the node.
      if (node.Append(bulk) == StatusError)
	escape("unable to append the node");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3>
    Status		JSON::Document::Append(const T1&	id1,
					       const T2&	id2,
					       const T3&	id3,
					       const Bulk&	bulk)
    {
      Node		node;

      enter();

      // dig the way to the target.
      if (this->Dig(id1, id2, id3,
		    node) == StatusError)
	escape("unable to dig the way to the target");

      // append the bulk to the node.
      if (node.Append(bulk) == StatusError)
	escape("unable to append the node");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4>
    Status		JSON::Document::Append(const T1&	id1,
					       const T2&	id2,
					       const T3&	id3,
					       const T4&	id4,
					       const Bulk&	bulk)
    {
      Node		node;

      enter();

      // dig the way to the target.
      if (this->Dig(id1, id2, id3, id4,
		    node) == StatusError)
	escape("unable to dig the way to the target");

      // append the bulk to the node.
      if (node.Append(bulk) == StatusError)
	escape("unable to append the node");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5>
    Status		JSON::Document::Append(const T1&	id1,
					       const T2&	id2,
					       const T3&	id3,
					       const T4&	id4,
					       const T5&	id5,
					       const Bulk&	bulk)
    {
      Node		node;

      enter();

      // dig the way to the target.
      if (this->Dig(id1, id2, id3, id4, id5,
		    node) == StatusError)
	escape("unable to dig the way to the target");

      // append the bulk to the node.
      if (node.Append(bulk) == StatusError)
	escape("unable to append the node");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6>
    Status		JSON::Document::Append(const T1&	id1,
					       const T2&	id2,
					       const T3&	id3,
					       const T4&	id4,
					       const T5&	id5,
					       const T6&	id6,
					       const Bulk&	bulk)
    {
      Node		node;

      enter();

      // dig the way to the target.
      if (this->Dig(id1, id2, id3, id4, id5, id6,
		    node) == StatusError)
	escape("unable to dig the way to the target");

      // append the bulk to the node.
      if (node.Append(bulk) == StatusError)
	escape("unable to append the node");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7>
    Status		JSON::Document::Append(const T1&	id1,
					       const T2&	id2,
					       const T3&	id3,
					       const T4&	id4,
					       const T5&	id5,
					       const T6&	id6,
					       const T7&	id7,
					       const Bulk&	bulk)
    {
      Node		node;

      enter();

      // dig the way to the target.
      if (this->Dig(id1, id2, id3, id4, id5, id6, id7,
		    node) == StatusError)
	escape("unable to dig the way to the target");

      // append the bulk to the node.
      if (node.Append(bulk) == StatusError)
	escape("unable to append the node");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8>
    Status		JSON::Document::Append(const T1&	id1,
					       const T2&	id2,
					       const T3&	id3,
					       const T4&	id4,
					       const T5&	id5,
					       const T6&	id6,
					       const T7&	id7,
					       const T8&	id8,
					       const Bulk&	bulk)
    {
      Node		node;

      enter();

      // dig the way to the target.
      if (this->Dig(id1, id2, id3, id4, id5, id6, id7, id8,
		    node) == StatusError)
	escape("unable to dig the way to the target");

      // append the bulk to the node.
      if (node.Append(bulk) == StatusError)
	escape("unable to append the node");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8,
	      typename T9>
    Status		JSON::Document::Append(const T1&	id1,
					       const T2&	id2,
					       const T3&	id3,
					       const T4&	id4,
					       const T5&	id5,
					       const T6&	id6,
					       const T7&	id7,
					       const T8&	id8,
					       const T9&	id9,
					       const Bulk&	bulk)
    {
      Node		node;

      enter();

      // dig the way to the target.
      if (this->Dig(id1, id2, id3, id4, id5, id6, id7, id8, id9,
		    node) == StatusError)
	escape("unable to dig the way to the target");

      // append the bulk to the node.
      if (node.Append(bulk) == StatusError)
	escape("unable to append the node");

      leave();
    }

    template <typename T1>
    Status		JSON::Document::Erase(const T1&		id1)
    {
      Node		node;

      enter();

      // erase the identified member from the root node.
      if (this->root.Erase(id1) == StatusError)
	escape("unable to erase the member");

      leave();
    }

    template <typename T1,
	      typename T2>
    Status		JSON::Document::Erase(const T1&		id1,
					      const T2&		id2)
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1,
		    node) == StatusError)
	escape("unable to retrieve the node");

      // erase the identified member.
      if (node.Erase(id2) == StatusError)
	escape("unable to erase the member");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3>
    Status		JSON::Document::Erase(const T1&		id1,
					      const T2&		id2,
					      const T3&		id3)
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2,
		    node) == StatusError)
	escape("unable to retrieve the node");

      // erase the identified member.
      if (node.Erase(id3) == StatusError)
	escape("unable to erase the member");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4>
    Status		JSON::Document::Erase(const T1&		id1,
					      const T2&		id2,
					      const T3&		id3,
					      const T4&		id4)
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2, id3,
		    node) == StatusError)
	escape("unable to retrieve the node");

      // erase the identified member.
      if (node.Erase(id4) == StatusError)
	escape("unable to erase the member");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5>
    Status		JSON::Document::Erase(const T1&		id1,
					      const T2&		id2,
					      const T3&		id3,
					      const T4&		id4,
					      const T5&		id5)
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2, id3, id4,
		    node) == StatusError)
	escape("unable to retrieve the node");

      // erase the identified member.
      if (node.Erase(id5) == StatusError)
	escape("unable to erase the member");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6>
    Status		JSON::Document::Erase(const T1&		id1,
					      const T2&		id2,
					      const T3&		id3,
					      const T4&		id4,
					      const T5&		id5,
					      const T6&		id6)
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2, id3, id4, id5,
		    node) == StatusError)
	escape("unable to retrieve the node");

      // erase the identified member.
      if (node.Erase(id6) == StatusError)
	escape("unable to erase the member");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7>
    Status		JSON::Document::Erase(const T1&		id1,
					      const T2&		id2,
					      const T3&		id3,
					      const T4&		id4,
					      const T5&		id5,
					      const T6&		id6,
					      const T7&		id7)
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2, id3, id4, id5, id6,
		    node) == StatusError)
	escape("unable to retrieve the node");

      // erase the identified member.
      if (node.Erase(id7) == StatusError)
	escape("unable to erase the member");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8>
    Status		JSON::Document::Erase(const T1&		id1,
					      const T2&		id2,
					      const T3&		id3,
					      const T4&		id4,
					      const T5&		id5,
					      const T6&		id6,
					      const T7&		id7,
					      const T8&		id8)
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2, id3, id4, id5, id6, id7,
		    node) == StatusError)
	escape("unable to retrieve the node");

      // erase the identified member.
      if (node.Erase(id8) == StatusError)
	escape("unable to erase the member");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8,
	      typename T9>
    Status		JSON::Document::Erase(const T1&		id1,
					      const T2&		id2,
					      const T3&		id3,
					      const T4&		id4,
					      const T5&		id5,
					      const T6&		id6,
					      const T7&		id7,
					      const T8&		id8,
					      const T9&		id9)
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2, id3, id4, id5, id6, id7, id8,
		    node) == StatusError)
	escape("unable to retrieve the node");

      // erase the identified member.
      if (node.Erase(id9) == StatusError)
	escape("unable to erase the member");

      leave();
    }

    template <typename T1>
    Status		JSON::Document::Clear(const T1&		id1)
    {
      Node		node;

      enter();

      // clear the identified member from the root node.
      if (this->root.Clear(id1) == StatusError)
	escape("unable to clear the member");

      leave();
    }

    template <typename T1,
	      typename T2>
    Status		JSON::Document::Clear(const T1&		id1,
					      const T2&		id2)
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1,
		    node) == StatusError)
	escape("unable to retrieve the node");

      // clear the identified member.
      if (node.Clear(id2) == StatusError)
	escape("unable to clear the member");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3>
    Status		JSON::Document::Clear(const T1&		id1,
					      const T2&		id2,
					      const T3&		id3)
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2,
		    node) == StatusError)
	escape("unable to retrieve the node");

      // clear the identified member.
      if (node.Clear(id3) == StatusError)
	escape("unable to clear the member");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4>
    Status		JSON::Document::Clear(const T1&		id1,
					      const T2&		id2,
					      const T3&		id3,
					      const T4&		id4)
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2, id3,
		    node) == StatusError)
	escape("unable to retrieve the node");

      // clear the identified member.
      if (node.Clear(id4) == StatusError)
	escape("unable to clear the member");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5>
    Status		JSON::Document::Clear(const T1&		id1,
					      const T2&		id2,
					      const T3&		id3,
					      const T4&		id4,
					      const T5&		id5)
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2, id3, id4,
		    node) == StatusError)
	escape("unable to retrieve the node");

      // clear the identified member.
      if (node.Clear(id5) == StatusError)
	escape("unable to clear the member");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6>
    Status		JSON::Document::Clear(const T1&		id1,
					      const T2&		id2,
					      const T3&		id3,
					      const T4&		id4,
					      const T5&		id5,
					      const T6&		id6)
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2, id3, id4, id5,
		    node) == StatusError)
	escape("unable to retrieve the node");

      // clear the identified member.
      if (node.Clear(id6) == StatusError)
	escape("unable to clear the member");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7>
    Status		JSON::Document::Clear(const T1&		id1,
					      const T2&		id2,
					      const T3&		id3,
					      const T4&		id4,
					      const T5&		id5,
					      const T6&		id6,
					      const T7&		id7)
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2, id3, id4, id5, id6,
		    node) == StatusError)
	escape("unable to retrieve the node");

      // clear the identified member.
      if (node.Clear(id7) == StatusError)
	escape("unable to clear the member");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8>
    Status		JSON::Document::Clear(const T1&		id1,
					      const T2&		id2,
					      const T3&		id3,
					      const T4&		id4,
					      const T5&		id5,
					      const T6&		id6,
					      const T7&		id7,
					      const T8&		id8)
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2, id3, id4, id5, id6, id7,
		    node) == StatusError)
	escape("unable to retrieve the node");

      // clear the identified member.
      if (node.Clear(id8) == StatusError)
	escape("unable to clear the member");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8,
	      typename T9>
    Status		JSON::Document::Clear(const T1&		id1,
					      const T2&		id2,
					      const T3&		id3,
					      const T4&		id4,
					      const T5&		id5,
					      const T6&		id6,
					      const T7&		id7,
					      const T8&		id8,
					      const T9&		id9)
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2, id3, id4, id5, id6, id7, id8,
		    node) == StatusError)
	escape("unable to retrieve the node");

      // clear the identified member.
      if (node.Clear(id9) == StatusError)
	escape("unable to clear the member");

      leave();
    }

    template <typename T1>
    Status		JSON::Document::Size(const T1&		id1,
					     Natural32&		size)
    {
      Node		node;

      enter();

      // size the identified member from the root node.
      if (this->root.Size(id1) == StatusError)
	escape("unable to size the member");

      leave();
    }

    template <typename T1,
	      typename T2>
    Status		JSON::Document::Size(const T1&		id1,
					     const T2&		id2,
					     Natural32&		size)
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1,
		    node) == StatusError)
	escape("unable to retrieve the node");

      // size the identified member.
      if (node.Size(id2) == StatusError)
	escape("unable to size the member");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3>
    Status		JSON::Document::Size(const T1&		id1,
					     const T2&		id2,
					     const T3&		id3,
					     Natural32&		size)
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2,
		    node) == StatusError)
	escape("unable to retrieve the node");

      // size the identified member.
      if (node.Size(id3) == StatusError)
	escape("unable to size the member");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4>
    Status		JSON::Document::Size(const T1&		id1,
					     const T2&		id2,
					     const T3&		id3,
					     const T4&		id4,
					     Natural32&		size)
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2, id3,
		    node) == StatusError)
	escape("unable to retrieve the node");

      // size the identified member.
      if (node.Size(id4) == StatusError)
	escape("unable to size the member");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5>
    Status		JSON::Document::Size(const T1&		id1,
					     const T2&		id2,
					     const T3&		id3,
					     const T4&		id4,
					     const T5&		id5,
					     Natural32&		size)
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2, id3, id4,
		    node) == StatusError)
	escape("unable to retrieve the node");

      // size the identified member.
      if (node.Size(id5) == StatusError)
	escape("unable to size the member");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6>
    Status		JSON::Document::Size(const T1&		id1,
					     const T2&		id2,
					     const T3&		id3,
					     const T4&		id4,
					     const T5&		id5,
					     const T6&		id6,
					     Natural32&		size)
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2, id3, id4, id5,
		    node) == StatusError)
	escape("unable to retrieve the node");

      // size the identified member.
      if (node.Size(id6) == StatusError)
	escape("unable to size the member");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7>
    Status		JSON::Document::Size(const T1&		id1,
					     const T2&		id2,
					     const T3&		id3,
					     const T4&		id4,
					     const T5&		id5,
					     const T6&		id6,
					     const T7&		id7,
					     Natural32&		size)
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2, id3, id4, id5, id6,
		    node) == StatusError)
	escape("unable to retrieve the node");

      // size the identified member.
      if (node.Size(id7) == StatusError)
	escape("unable to size the member");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8>
    Status		JSON::Document::Size(const T1&		id1,
					     const T2&		id2,
					     const T3&		id3,
					     const T4&		id4,
					     const T5&		id5,
					     const T6&		id6,
					     const T7&		id7,
					     const T8&		id8,
					     Natural32&		size)
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2, id3, id4, id5, id6, id7,
		    node) == StatusError)
	escape("unable to retrieve the node");

      // size the identified member.
      if (node.Size(id8) == StatusError)
	escape("unable to size the member");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8,
	      typename T9>
    Status		JSON::Document::Size(const T1&		id1,
					     const T2&		id2,
					     const T3&		id3,
					     const T4&		id4,
					     const T5&		id5,
					     const T6&		id6,
					     const T7&		id7,
					     const T8&		id8,
					     const T9&		id9,
					     Natural32&		size)
    {
      Node		node;

      enter();

      // retrieve the node given the identifiers.
      if (this->Get(id1, id2, id3, id4, id5, id6, id7, id8,
		    node) == StatusError)
	escape("unable to retrieve the node");

      // size the identified member.
      if (node.Size(id9) == StatusError)
	escape("unable to size the member");

      leave();
    }

  }
}

#endif
