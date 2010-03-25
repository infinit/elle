//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/misc/Function.hxx
//
// created       julien quintard   [thu feb  4 22:18:05 2010]
// updated       julien quintard   [thu mar 25 00:26:06 2010]
//

#ifndef ELLE_MISC_FUNCTION_HXX
#define ELLE_MISC_FNUCTION_HXX

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
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor.
    ///
    template <typename... T>
    Function<T...>::Function(Handler				handler):
      Routine::Routine(Routine::TypeFunction),

      handler(handler)
    {
    }

    ///
    /// the copy constructor.
    ///
    template <typename... T>
    Function<T...>::Function(const Function<T...>&		function):
      handler(function.handler)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method triggers the handler by considering arguments as pointers:
    /// especially required by entrances.
    ///
    template <typename... T>
    Status		Function<T...>::Trigger(T*...		arguments)
    {
      return (this->handler(arguments...));
    }

    ///
    /// this method calls the handler by considering arguments as references:
    /// especially required by callbacks.
    ///
    template <typename... T>
    Status		Function<T...>::Call(T&...		arguments)
    {
      return (this->handler(arguments...));
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the entity.
    ///
    embed(Entity, Function<T...>, template <typename... T>);

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
      String		shift(2, ' ');

      enter();

      std::cout << alignment << "[Function]" << std::endl;

      // dump the quantum.
      std::cout << alignment << shift << "[Quantum] "
		<< std::dec << sizeof...(T) << std::endl;

      // dump the handler.
      std::cout << alignment << shift << "[Handler] "
		<< std::hex << this->handler << std::endl;

      leave();
    }

  }
}

#endif
