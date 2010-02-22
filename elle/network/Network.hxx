//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Network.hxx
//
// created       julien quintard   [wed feb  3 16:05:34 2010]
// updated       julien quintard   [sun feb 21 19:50:48 2010]
//

#ifndef ELLE_NETWORK_NETWORK_HXX
#define ELLE_NETWORK_NETWORK_HXX

namespace elle
{
  namespace network
  {

//
// ---------- network ---------------------------------------------------------
//

    ///
    /// this method registers a callback associated with a set of types so that
    /// whenever a message is received, these types are extracted and the
    /// callback is triggered.
    ///
    template <const Tag G, class P>
    Status		Network::Register(Callback&		callback)
    {
      Selectionoid<P::Count, P>*		selectionoid;
      std::pair<Network::Iterator, Boolean>	result;

      // check if this type has already been registed.
      if (Network::Callbacks.find(G) != Network::Callbacks.end())
	escape("unable to register an already registered type");

      // create a new selectionoid.
      selectionoid = new Selectionoid<P::Count, P>;

      // set the callback.
      selectionoid->callback = new Callback(callback);

      // insert the callback in the container.
      result = Network::Callbacks.insert(
		 std::pair<Tag,
		           Network::Functionoid*>(G,
						  selectionoid));

      // check if the insertion was successful.
      if (result.second == false)
	escape("unable to insert the callback in the container");

      leave();
    }

//
// ---------- selectonoids ----------------------------------------------------
//

    ///
    /// these selectionoids must be cloned depending on the number of
    /// parameters they take.
    ///

    ///
    /// empty selectionoid.
    ///
    template <class P>
    class Network::Selectionoid<0, P>:
      public Network::Functionoid
    {
    public:
      //
      // constructors & destructors
      //
      Selectionoid():
	callback(NULL)
      {
      }

      ~Selectionoid()
      {
	// delete the callback.
	if (this->callback != NULL)
	  delete this->callback;
      }

      //
      // methods
      //
      Status		Dispatch(Environment&			environment,
				 Archive&			archive) const
      {
	// trigger the callback.
	if (this->callback->Trigger(environment) == StatusError)
	  escape("unable to complete the callback");

	leave();
      }

      Status		Dump(const Natural32			margin) const
      {
	// dump the callback.
	if (this->callback->Dump(margin) == StatusError)
	  escape("unable to dump the callback");

	leave();
      }

      //
      // attributes
      //
      Callback*		callback;
    };

    ///
    /// single-parameter selectoinoid.
    ///
    template <class P>
    class Network::Selectionoid<1, P>:
      public Network::Functionoid
    {
    public:
      //
      // constructors & destructors
      //
      Selectionoid():
	callback(NULL)
      {
      }

      ~Selectionoid()
      {
	// delete the callback.
	if (this->callback != NULL)
	  delete this->callback;
      }

      //
      // methods
      //
      Status		Dispatch(Environment&			environment,
				 Archive&			archive) const
      {
	typename P::Parameter1	o1;

	// extract the objects.
	if (archive.Extract(o1) == StatusError)
	  escape("unable to extract the objects");

	// trigger the callback.
	if (this->callback->Trigger(environment,
				    o1) == StatusError)
	  escape("unable to complete the callback");

	leave();
      }

      Status		Dump(const Natural32			margin) const
      {
	// dump the callback.
	if (this->callback->Dump(margin) == StatusError)
	  escape("unable to dump the callback");

	leave();
      }

      //
      // attributes
      //
      Callback*		callback;
    };

    ///
    /// two-parameters selectoinoid.
    ///
    template <class P>
    class Network::Selectionoid<2, P>:
      public Network::Functionoid
    {
    public:
      //
      // constructors & destructors
      //
      Selectionoid():
	callback(NULL)
      {
      }

      ~Selectionoid()
      {
	// delete the callback.
	if (this->callback != NULL)
	  delete this->callback;
      }

      //
      // methods
      //
      Status		Dispatch(Environment&			environment,
				 Archive&			archive) const
      {
	typename P::Parameter1		o1;
	typename P::Parameter2		o2;

	// extract the objects.
	if (archive.Extract(o1, o2) == StatusError)
	  escape("unable to extract the objects");

	// trigger the callback.
	if (this->callback->Trigger(environment,
				    o1, o2) == StatusError)
	  escape("unable to complete the callback");

	leave();
      }

      Status		Dump(const Natural32			margin) const
      {
	// dump the callback.
	if (this->callback->Dump(margin) == StatusError)
	  escape("unable to dump the callback");

	leave();
      }

      //
      // attributes
      //
      Callback*		callback;
    };

    ///
    /// three-parameters selectoinoid.
    ///
    template <class P>
    class Network::Selectionoid<3, P>:
      public Network::Functionoid
    {
    public:
      //
      // constructors & destructors
      //
      Selectionoid():
	callback(NULL)
      {
      }

      ~Selectionoid()
      {
	// delete the callback.
	if (this->callback != NULL)
	  delete this->callback;
      }

      //
      // methods
      //
      Status		Dispatch(Environment&			environment,
				 Archive&			archive) const
      {
	typename P::Parameter1		o1;
	typename P::Parameter2		o2;
	typename P::Parameter3		o3;

	// extract the objects.
	if (archive.Extract(o1, o2, o3) == StatusError)
	  escape("unable to extract the objects");

	// trigger the callback.
	if (this->callback->Trigger(environment,
				    o1, o2, o3) == StatusError)
	  escape("unable to complete the callback");

	leave();
      }

      Status		Dump(const Natural32			margin) const
      {
	// dump the callback.
	if (this->callback->Dump(margin) == StatusError)
	  escape("unable to dump the callback");

	leave();
      }

      //
      // attributes
      //
      Callback*		callback;
    };

    ///
    /// four-parameters selectoinoid.
    ///
    template <class P>
    class Network::Selectionoid<4, P>:
      public Network::Functionoid
    {
    public:
      //
      // constructors & destructors
      //
      Selectionoid():
	callback(NULL)
      {
      }

      ~Selectionoid()
      {
	// delete the callback.
	if (this->callback != NULL)
	  delete this->callback;
      }

      //
      // methods
      //
      Status		Dispatch(Environment&			environment,
				 Archive&			archive) const
      {
	typename P::Parameter1		o1;
	typename P::Parameter2		o2;
	typename P::Parameter3		o3;
	typename P::Parameter4		o4;

	// extract the objects.
	if (archive.Extract(o1, o2, o3, o4) == StatusError)
	  escape("unable to extract the objects");

	// trigger the callback.
	if (this->callback->Trigger(environment,
				    o1, o2, o3, o4) == StatusError)
	  escape("unable to complete the callback");

	leave();
      }

      Status		Dump(const Natural32			margin) const
      {
	// dump the callback.
	if (this->callback->Dump(margin) == StatusError)
	  escape("unable to dump the callback");

	leave();
      }

      //
      // attributes
      //
      Callback*		callback;
    };

    ///
    /// five-parameters selectoinoid.
    ///
    template <class P>
    class Network::Selectionoid<5, P>:
      public Network::Functionoid
    {
    public:
      //
      // constructors & destructors
      //
      Selectionoid():
	callback(NULL)
      {
      }

      ~Selectionoid()
      {
	// delete the callback.
	if (this->callback != NULL)
	  delete this->callback;
      }

      //
      // methods
      //
      Status		Dispatch(Environment&			environment,
				 Archive&			archive) const
      {
	typename P::Parameter1		o1;
	typename P::Parameter2		o2;
	typename P::Parameter3		o3;
	typename P::Parameter4		o4;
	typename P::Parameter5		o5;

	// extract the objects.
	if (archive.Extract(o1, o2, o3, o4, o5) == StatusError)
	  escape("unable to extract the objects");

	// trigger the callback.
	if (this->callback->Trigger(environment,
				    o1, o2, o3, o4, o5) == StatusError)
	  escape("unable to complete the callback");

	leave();
      }

      Status		Dump(const Natural32			margin) const
      {
	// dump the callback.
	if (this->callback->Dump(margin) == StatusError)
	  escape("unable to dump the callback");

	leave();
      }

      //
      // attributes
      //
      Callback*		callback;
    };

    ///
    /// six-parameters selectoinoid.
    ///
    template <class P>
    class Network::Selectionoid<6, P>:
      public Network::Functionoid
    {
    public:
      //
      // constructors & destructors
      //
      Selectionoid():
	callback(NULL)
      {
      }

      ~Selectionoid()
      {
	// delete the callback.
	if (this->callback != NULL)
	  delete this->callback;
      }

      //
      // methods
      //
      Status		Dispatch(Environment&			environment,
				 Archive&			archive) const
      {
	typename P::Parameter1		o1;
	typename P::Parameter2		o2;
	typename P::Parameter3		o3;
	typename P::Parameter4		o4;
	typename P::Parameter5		o5;
	typename P::Parameter6		o6;

	// extract the objects.
	if (archive.Extract(o1, o2, o3, o4, o5, o6) == StatusError)
	  escape("unable to extract the objects");

	// trigger the callback.
	if (this->callback->Trigger(environment,
				    o1, o2, o3, o4, o5, o6) == StatusError)
	  escape("unable to complete the callback");

	leave();
      }

      Status		Dump(const Natural32			margin) const
      {
	// dump the callback.
	if (this->callback->Dump(margin) == StatusError)
	  escape("unable to dump the callback");

	leave();
      }

      //
      // attributes
      //
      Callback*		callback;
    };

    ///
    /// six-parameters selectoinoid.
    ///
    template <class P>
    class Network::Selectionoid<7, P>:
      public Network::Functionoid
    {
    public:
      //
      // constructors & destructors
      //
      Selectionoid():
	callback(NULL)
      {
      }

      ~Selectionoid()
      {
	// delete the callback.
	if (this->callback != NULL)
	  delete this->callback;
      }

      //
      // methods
      //
      Status		Dispatch(Environment&			environment,
				 Archive&			archive) const
      {
	typename P::Parameter1		o1;
	typename P::Parameter2		o2;
	typename P::Parameter3		o3;
	typename P::Parameter4		o4;
	typename P::Parameter5		o5;
	typename P::Parameter6		o6;
	typename P::Parameter7		o7;

	// extract the objects.
	if (archive.Extract(o1, o2, o3, o4, o5, o6, o7) == StatusError)
	  escape("unable to extract the objects");

	// trigger the callback.
	if (this->callback->Trigger(environment,
				    o1, o2, o3, o4, o5, o6, o7) == StatusError)
	  escape("unable to complete the callback");

	leave();
      }

      Status		Dump(const Natural32			margin) const
      {
	// dump the callback.
	if (this->callback->Dump(margin) == StatusError)
	  escape("unable to dump the callback");

	leave();
      }

      //
      // attributes
      //
      Callback*		callback;
    };

    ///
    /// six-parameters selectoinoid.
    ///
    template <class P>
    class Network::Selectionoid<8, P>:
      public Network::Functionoid
    {
    public:
      //
      // constructors & destructors
      //
      Selectionoid():
	callback(NULL)
      {
      }

      ~Selectionoid()
      {
	// delete the callback.
	if (this->callback != NULL)
	  delete this->callback;
      }

      //
      // methods
      //
      Status		Dispatch(Environment&			environment,
				 Archive&			archive) const
      {
	typename P::Parameter1		o1;
	typename P::Parameter2		o2;
	typename P::Parameter3		o3;
	typename P::Parameter4		o4;
	typename P::Parameter5		o5;
	typename P::Parameter6		o6;
	typename P::Parameter7		o7;
	typename P::Parameter8		o8;

	// extract the objects.
	if (archive.Extract(o1, o2, o3, o4, o5, o6, o7, o8) == StatusError)
	  escape("unable to extract the objects");

	// trigger the callback.
	if (this->callback->Trigger(environment,
				    o1, o2, o3, o4, o5, o6, o7, o8) ==
	    StatusError)
	  escape("unable to complete the callback");

	leave();
      }

      Status		Dump(const Natural32			margin) const
      {
	// dump the callback.
	if (this->callback->Dump(margin) == StatusError)
	  escape("unable to dump the callback");

	leave();
      }

      //
      // attributes
      //
      Callback*		callback;
    };

    ///
    /// six-parameters selectoinoid.
    ///
    template <class P>
    class Network::Selectionoid<9, P>:
      public Network::Functionoid
    {
    public:
      //
      // constructors & destructors
      //
      Selectionoid():
	callback(NULL)
      {
      }

      ~Selectionoid()
      {
	// delete the callback.
	if (this->callback != NULL)
	  delete this->callback;
      }

      //
      // methods
      //
      Status		Dispatch(Environment&			environment,
				 Archive&			archive) const
      {
	typename P::Parameter1		o1;
	typename P::Parameter2		o2;
	typename P::Parameter3		o3;
	typename P::Parameter4		o4;
	typename P::Parameter5		o5;
	typename P::Parameter6		o6;
	typename P::Parameter7		o7;
	typename P::Parameter8		o8;
	typename P::Parameter9		o9;

	// extract the objects.
	if (archive.Extract(o1, o2, o3, o4, o5, o6, o7, o8, o9) == StatusError)
	  escape("unable to extract the objects");

	// trigger the callback.
	if (this->callback->Trigger(environment,
				    o1, o2, o3, o4, o5, o6, o7, o8, o9) ==
	    StatusError)
	  escape("unable to complete the callback");

	leave();
      }

      Status		Dump(const Natural32			margin) const
      {
	// dump the callback.
	if (this->callback->Dump(margin) == StatusError)
	  escape("unable to dump the callback");

	leave();
      }

      //
      // attributes
      //
      Callback*		callback;
    };

  }
}

#endif
