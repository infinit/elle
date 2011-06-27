//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Function.hxx
//
// created       julien quintard   [thu feb  4 22:18:05 2010]
// updated       julien quintard   [mon jun 27 07:17:47 2011]
//

#ifndef ELLE_CONCURRENCY_FUNCTION_HXX
#define ELLE_CONCURRENCY_FNUCTION_HXX

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
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor.
    ///
    template <typename... T>
    Function<T...>::Function(Handler				handler):
      handler(handler)
    {
    }

    ///
    /// the copy constructor.
    ///
    template <typename... T>
    Function<T...>::Function(const Function<T...>&		function):
      Object(function),

      handler(function.handler)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method calls the handler.
    ///
    template <typename... T>
    Status		Function<T...>::Call(T...		arguments)
    {
      return (this->handler(arguments...));
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Function<T...>, _(template <typename... T>));

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the function state.
    ///
    template <typename... T>
    Status		Function<T...>::Dump(const Natural32	margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Function]" << std::endl;

      // dump the quantum.
      std::cout << alignment << Dumpable::Shift << "[Quantum] "
		<< std::dec << sizeof... (T) << std::endl;

      // dump the handler.
      std::cout << alignment << Dumpable::Shift << "[Handler] "
		<< std::hex << this->handler << std::endl;

      leave();
    }

  }
}

#endif
