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
// updated       julien quintard   [sun feb 28 13:34:27 2010]
//

#ifndef ELLE_MISC_FUNCTION_HXX
#define ELLE_MISC_FNUCTION_HXX

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
    Function<T...>::Function(Function::Handler			handler):
      Callback::Callback(Callback::TypeFunction),

      handler(handler)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method actually calls the handler.
    ///
    template <typename... T>
    Status		Function<T...>::Call(T&...		parameters)
    {
      return (this->handler(parameters...));
    }

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
