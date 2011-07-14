//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Method.hxx
//
// created       julien quintard   [thu feb  4 23:08:34 2010]
// updated       julien quintard   [thu jul 14 14:38:28 2011]
//

#ifndef ELLE_CONCURRENCY_METHOD_HXX
#define ELLE_CONCURRENCY_METHOD_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/String.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>
 
namespace elle
{
  namespace concurrency
  {

//
// ---------- wrap ------------------------------------------------------------
//

    ///
    /// the default constructor.
    ///
    template <typename... T>
    template <typename C>
    Method< Parameters<T...> >::Wrap<C>::Wrap(Handler		handler,
					      C*		object):
      object(object),
      handler(handler)
    {
    }

    ///
    /// this method calls the handler.
    ///
    template <typename... T>
    template <typename C>
    Status
    Method< Parameters<T...> >::Wrap<C>::Call(T...		arguments)
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
    Status
    Method< Parameters<T...> >::Wrap<C>::Dump(const Natural32	margin)
      const
    {
      String		alignment(margin, ' ');

      enter();

      // dump the quantum.
      std::cout << alignment << Dumpable::Shift << "[Quantum] "
		<< std::dec << sizeof... (T) << std::endl;

      // dump the object.
      std::cout << alignment << Dumpable::Shift << "[Object] "
		<< std::hex << this->object << std::endl;

      // dump the handler.
      std::cout << alignment << Dumpable::Shift << "[Handler] "
		<< std::hex << this->handler << std::endl;

      leave();
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Method< Parameters<T...> >::Wrap<C>,
	  _(template <typename... T>
	    template <typename C>));

//
// ---------- method ----------------------------------------------------------
//

    ///
    /// the default constructor.
    ///
    template <typename... T>
    template <typename C>
    Method< Parameters<T...> >::Method(Status		(C::*handler)(T...),
				       C*		object):
      shell(new Method< Parameters<T...> >::Wrap<C>(handler, object))
    {
    }

    ///
    /// the copy constructor.
    ///
    template <typename... T>
    Method< Parameters<T...> >::Method(
				  const Method< Parameters<T...> >& method):
      Object(method)
    {
      // clone the shell.
      //
      // note that the Clone method must be called because at this point,
      // this constructor does not know the type of the _method_'s wrap.
      if (method.shell != NULL)
	method.shell->Clone((Object*&)this->shell);
    }

    ///
    /// destructor.
    ///
    template <typename... T>
    Method< Parameters<T...> >::~Method()
    {
      // delete the shell, if present.
      if (this->shell != NULL)
	delete this->shell;
    }

    ///
    /// this method calls the handler.
    ///
    template <typename... T>
    Status
    Method< Parameters<T...> >::Call(T...			arguments)
    {
      return (this->shell->Call(arguments...));
    }

    ///
    /// this method dumps the method state.
    ///
    template <typename... T>
    Status
    Method< Parameters<T...> >::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Method]" << std::endl;

      if (this->shell->Dump(margin) == StatusError)
	escape("unable to dump the shell");

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// these are generated automatically.
    ///
    embed(Method< Parameters<T...> >,
	  _(template <typename... T>));

  }
}

#endif
