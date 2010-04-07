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
// updated       julien quintard   [mon mar 29 21:51:10 2010]
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
    /// this method calls the handler.
    ///
    template <typename... T>
    Status		Function<T...>::Call(T...		arguments)
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

      enter();

      std::cout << alignment << "[Function]" << std::endl;

      // dump the quantum.
      std::cout << alignment << Dumpable::Shift << "[Quantum] "
		<< std::dec << sizeof...(T) << std::endl;

      // dump the handler.
      std::cout << alignment << Dumpable::Shift << "[Handler] "
		<< std::hex << this->handler << std::endl;

      leave();
    }

  }
}

#endif
