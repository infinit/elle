//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/misc/Method.hxx
//
// created       julien quintard   [thu feb  4 23:08:34 2010]
// updated       julien quintard   [wed mar 17 00:35:02 2010]
//

#ifndef ELLE_MISC_METHOD_HXX
#define ELLE_MISC_METHOD_HXX

namespace elle
{
  namespace misc
  {

//
// ---------- method ----------------------------------------------------------
//

    ///
    /// the default constructor.
    ///
    template <typename... T>
    template <typename U>
    Method<T...>::Wrap<U>::Wrap(U*				object,
				Method<T...>::Wrap<U>::Handler	handler):
      object(object),
      handler(handler)
    {
    }

    ///
    /// this method actually calls the handler.
    ///
    template <typename... T>
    template <typename U>
    Status		Method<T...>::Wrap<U>::Call(T&...	arguments)
    {
      enter();

      // trigger the callback.
      if ((this->object->*this->handler)(arguments...) == StatusError)
	escape("an error occured in the callback");

      leave();
    }

    ///
    /// this method dumps the method state.
    ///
    template <typename... T>
    template <typename U>
    Status		Method<T...>::Wrap<U>::Dump(const Natural32	margin)
      const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      enter();

      // dump the quantum.
      std::cout << alignment << shift << "[Quantum] "
		<< std::dec << sizeof...(T) << std::endl;

      // dump the object.
      std::cout << alignment << shift << "[Object] "
		<< std::hex << this->object << std::endl;

      // dump the handler.
      std::cout << alignment << shift << "[Handler] "
		<< std::hex << this->handler << std::endl;

      leave();
    }

//
// ---------- method ----------------------------------------------------------
//

    ///
    /// the default constructor.
    ///
    template <typename... T>
    template <typename X>
    Method<T...>::Method(X*				object,
			 Status				(X::*handler)(T&...)):
      Callback::Callback(Callback::TypeMethod),

      shell(NULL)
    {
      this->shell = new Method::Wrap<X>(object, handler);
    }

    ///
    /// this method actually calls the handler.
    ///
    template <typename... T>
    Status		Method<T...>::Call(T&...		arguments)
    {
      return (this->shell->Call(arguments...));
    }

    ///
    /// this method dumps the method state.
    ///
    template <typename... T>
    Status		Method<T...>::Dump(const Natural32	margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      enter();

      std::cout << alignment << "[Method]" << std::endl;

      if (this->shell->Dump(margin) == StatusError)
	escape("unable to dump the shell");

      leave();
    }

  }
}

#endif
