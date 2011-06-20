//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/radix/Trace.cc
//
// created       julien quintard   [mon apr 26 21:25:23 2010]
// updated       julien quintard   [sun jun 19 22:29:28 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Trace.hh>

#include <elle/core/Integer.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  namespace radix
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Trace::Trace(const Void*					address):
      address(address),
      size(0)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method generates a trace for the current stack.
    ///
    Void		Trace::Generate()
    {
      // retrieve the frame addresses.
      this->size = ::backtrace(this->frames, Trace::Capacity);
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the trace.
    ///
    Status		Trace::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      char**		symbols;
      Natural32		i;

      enter();

      std::cout << alignment << "[Trace]" << std::endl;

      // dump the address.
      std::cout << alignment << Dumpable::Shift << "[Address] "
		<< this->address << std::endl;

      std::cout << alignment << Dumpable::Shift << "[Stack] " << std::endl;

      // transform the frames into human-kind-of-readable strings.
      if ((symbols = ::backtrace_symbols(this->frames, this->size)) == NULL)
	escape("unable to resolve the frame frames into symbols");

      // go through the symbols, starting with 1 in order to
      // ignore the last call which is irrelevant to the trace.
      for (i = 1; i < this->size; i++)
	{
	  String	line(symbols[i]);
	  Character*	symbol;
	  Integer32	first;
	  Integer32	last;
	  Integer32	index;
	  int		status;

	  // find a parenthese, indicating the beginning of the mangled name.
	  if ((index = line.find_first_of("(")) == String::npos)
	    {
	      // display the identifier, as such.
	      std::cout << alignment << Dumpable::Shift << Dumpable::Shift
			<< line << std::endl;

	      continue;
	    }

	  // set the first index as following the parenthese.
	  first = index;

	  // find the + character, indicating the end of the mangled name.
	  if ((index = line.find_first_of("+", first)) == String::npos)
	    {
	      // display the identifier, as such.
	      std::cout << alignment << Dumpable::Shift << Dumpable::Shift
			<< line << std::endl;

	      continue;
	    }

	  // set the last index as precedin the + character.
	  last = index;

	  // ignore if the name is empty.
	  if ((last - 1) == first)
	    {
	      // display the identifier, as such.
	      std::cout << alignment << Dumpable::Shift << Dumpable::Shift
			<< line << std::endl;

	      continue;
	    }

	  // initialize the symbol.
	  symbol = NULL;

	  // try to demangle the name.
	  symbol = abi::__cxa_demangle(line.substr(first + 1,
						   last - 1 - first).c_str(),
				       NULL,
				       NULL,
				       &status);

	  // look at the result of the demangling process.
	  switch (status)
	    {
	    case -1:
	    case -3:
	      {
		// release the symbol, if set.
		if (symbol != NULL)
		  ::free(symbol);

		// release the symbols array.
		::free(symbols);

		// an error occured.
		escape("an error occured while demangling a symbol");
	      }
	    case -2:
	      {
		// display the identifier, as such.
		std::cout << alignment << Dumpable::Shift << Dumpable::Shift
			  << line << std::endl;

		continue;
	      }
	    }

	  // display the demangled symbol.
	  std::cout << alignment << Dumpable::Shift << Dumpable::Shift
		    << symbol << std::endl;

	  // release the symbol's memory.
	  ::free(symbol);
	}

      // release the symbols array.
      ::free(symbols);

      leave();
    }

  }
}
