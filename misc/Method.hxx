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
// updated       julien quintard   [thu mar 25 00:32:54 2010]
//

#ifndef ELLE_MISC_METHOD_HXX
#define ELLE_MISC_METHOD_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/misc/Report.hh>
#include <elle/misc/Maid.hh>

namespace elle
{
  namespace misc
  {

//
// ---------- wrap ------------------------------------------------------------
//

    ///
    /// the default constructor.
    ///
    template <typename... T>
    template <typename C>
    Method<T...>::Wrap<C>::Wrap(C*				object,
				Method<T...>::Wrap<C>::Handler	handler):
      object(object),
      handler(handler)
    {
    }

    ///
    /// this method triggers the handler by considering arguments as pointers:
    /// required by entrances.
    ///
    template <typename... T>
    template <typename C>
    Status		Method<T...>::Wrap<C>::Trigger(T*...	arguments)
    {
      enter();

      // trigger the callback.
      if ((this->object->*this->handler)(arguments...) == StatusError)
	escape("an error occured in the callback");

      leave();
    }

    ///
    /// this method calls the handler by considering arguments as references:
    /// required by callbacks.
    ///
    template <typename... T>
    template <typename C>
    Status		Method<T...>::Wrap<C>::Call(T&...	arguments)
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
    template <typename C>
    Status		Method<T...>::Wrap<C>::Dump(const Natural32	margin)
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
			 Status				(X::*handler)(T...)):
      Routine::Routine(Routine::TypeMethod),

      shell(new Method::Wrap<X>(object, handler))
    {
    }

    ///
    /// this method triggers the handler by considering the arguments as
    /// pointers: required by entrances.
    ///
    template <typename... T>
    Status		Method<T...>::Trigger(T*...		arguments)
    {
      return (this->shell->Trigger(arguments...));
    }

    ///
    /// this method calls the handler by considering the arguments as
    /// references: required by callbacks.
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

    ///
    /// these are generated automatically.
    ///
    embed(Entity, Method<T...>, template <typename... T>);

  }
}

#endif
