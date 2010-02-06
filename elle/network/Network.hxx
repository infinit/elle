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
// updated       julien quintard   [sat feb  6 05:10:13 2010]
//

#ifndef ELLE_NETWORK_NETWORK_HXX
#define ELLE_NETWORK_NETWORK_HXX

namespace elle
{
  namespace network
  {

//
// ---------- transformer -----------------------------------------------------
//

    ///
    /// the default constructor.
    ///
    template <typename T>
    Network::Transformer<T>::Transformer():
      callback(NULL)
    {
    }

    ///
    /// the destructor releases the callback.
    ///
    template <typename T>
    Network::Transformer<T>::~Transformer()
    {
      // check if there is a callback.
      if (this->callback != NULL)
	delete this->callback;
    }

    ///
    /// XXX
    ///
    template <typename T>
    Status	Network::Transformer<T>::Dispatch(Environment&	environment,
						  Archive&	archive) const
    {
      T		message;

      // extract the archive, assuming the tag has already been extracting.
      if (archive.Extract(message) == StatusError)
	escape("unable to extract the message from the archive");

      // trigger the callback.
      if (this->callback->Trigger(environment, message) == StatusError)
	escape("unable to complete the callback");

      leave();
    }

    ///
    /// this method dumps the function callback.
    ///
    template <typename T>
    Status	Network::Transformer<T>::Dump(const Natural32	margin) const
    {
      // dump the callback.
      if (this->callback->Dump(margin) == StatusError)
	escape("unable to dump the callback");

      leave();
    }

//
// ---------- network ---------------------------------------------------------
//

    ///
    /// this method registers a callback associated with a type so that
    /// whenever a message of this type is received, the callback is
    /// triggered.
    ///
    template <typename T>
    Status		Network::Register(Callback*		callback)
    {
      Transformer<T>*				transformer;
      std::pair<Network::Iterator, Boolean>	result;

      // check if this type has already been registed.
      if (Network::Callbacks.find(T::Identifier) != Network::Callbacks.end())
	escape("unable to register an already registered type");

      // create a new transformer.
      transformer = new Transformer<T>;

      // set the callback.
      transformer->callback = callback;

      // insert the callback in the container.
      result = Network::Callbacks.insert(
		 std::pair<Tag,
		           Network::Functionoid*>(T::Identifier,
						  transformer));

      // check if the insertion was successful.
      if (result.second == false)
	escape("unable to insert the callback in the container");

      leave();
    }

  }
}

#endif
