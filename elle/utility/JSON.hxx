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
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
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

  }
}

#endif
