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
// updated       julien quintard   [thu feb  4 02:08:08 2010]
//

#ifndef ELLE_NETWORK_NETWORK_HXX
#define ELLE_NETWORK_NETWORK_HXX

namespace elle
{
  namespace network
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// default constructor.
    ///
    template <typename T>
    Network::CallbackFunction<T>::CallbackFunction(Status (*f)(Environment&,
							       T&)):
      Network::Functionoid(Network::Functionoid::TypeFunction),

      function(f)
    {
    }

    ///
    /// XXX
    ///
    template <typename T>
    Status	Network::CallbackFunction<T>::Trigger(Environment& environment,
						      Archive& archive) const
    {
      T		message;

      // extract the archive, assuming the tag has already been extracting.
      if (archive.Extract(message) == StatusError)
	escape("unable to extract the message from the archive");

      // trigger the callback.
      if (this->function(environment, message) == StatusError)
	escape("unable to complete the callback");

      leave();
    }

    ///
    /// this method dumps the function callback.
    ///
    template <typename T>
    Status	Network::CallbackFunction<T>::Dump(const Natural32 margin)
      const
    {
      String	alignment(margin, ' ');
      String	shift(2, ' ');

      std::cout << alignment << "[Callback] Function" << std::endl;

      // dump the function pointer.
      std::cout << alignment << shift << "[Function] "
		<< std::hex << this->function << std::endl;

      leave();
    }

    ///
    /// default constructor.
    ///
    template <typename T, class I>
    Network::CallbackMethod<T, I>::CallbackMethod(I* instance,
						  Status (I::*method)(Environment&,
								      T&)):
      Network::Functionoid(Network::Functionoid::TypeMethod),

      instance(instance),
      method(method)
    {
    }

    ///
    /// XXX
    ///
    template <typename T, class I>
    Status	Network::CallbackMethod<T, I>::Trigger(Environment& environment,
						       Archive& archive)
      const
    {
      T		message;

      // extract the archive, assuming the tag has already been extracting.
      if (archive.Extract(message) == StatusError)
	escape("unable to extract the message from the archive");

      // trigger the callback.
      if ((this->instance->*method)(environment, message) == StatusError)
	escape("unable to complete the callback");

      leave();
    }

    ///
    /// this method dumps the method callback.
    ///
    template <typename T, class I>
    Status	Network::CallbackMethod<T, I>::Dump(const Natural32 margin)
      const
    {
      String	alignment(margin, ' ');
      String	shift(2, ' ');

      std::cout << alignment << "[Callback] Method" << std::endl;

      // dump the instance pointer.
      std::cout << alignment << shift << "[Instance] "
		<< std::hex << this->instance << std::endl;

      // dump the method pointer.
      std::cout << alignment << shift << "[Method] "
		<< std::hex << this->method << std::endl;

      leave();
    }

    ///
    /// this method register a function callback associated to a tag so that
    /// whenever a message is received with this tag, the callback is
    /// triggered.
    ///
    template <typename T>
    Status		Network::Register(Status (*function)(Environment&,
							     T&))
    {
      // check if this type has already been registed.
      if (Network::Callbacks.find(T::Identifier) != Network::Callbacks.end())
	escape("unable to register an already registered type");

      // create a callback.
      Network::CallbackFunction<T>*	functionoid =
	new Network::CallbackFunction<T>(function);

      // insert the callback in the container.
      std::pair<Network::Iterator, Boolean> result =
	Network::Callbacks.insert(std::pair<Tag,
				            Network::Functionoid*>(T::Identifier,
								   functionoid));

      // check if the insertion was successful.
      if (result.second == false)
	escape("unable to insert the callback in the container");

      leave();
    }

    ///
    /// this method register a method callback associated to a tag so that
    /// whenever a message is received with this tag, the callback is
    /// triggered.
    ///
    template <typename T, class I>
    Status		Network::Register(I* instance,
					  Status (I::*method)(Environment&,
							      T&))
    {
      // check if this type has already been registed.
      if (Network::Callbacks.find(T::Identifier) != Network::Callbacks.end())
	escape("unable to register an already registered type");

      // create a callback.
      Network::CallbackMethod<T, I>*	functionoid =
	new Network::CallbackMethod<T, I>(instance, method);

      // insert the callback in the container.
      std::pair<Network::Iterator, Boolean> result =
	Network::Callbacks.insert(std::pair<Tag,
				            Network::Functionoid*>(T::Identifier,
								   functionoid));

      // check if the insertion was successful.
      if (result.second == false)
	escape("unable to insert the callback in the container");

      leave();
    }

  }
}

#endif
