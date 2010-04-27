//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/elle/core/Trace.cc
//
// created       julien quintard   [mon apr 26 21:25:23 2010]
// updated       julien quintard   [tue apr 27 17:29:32 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Trace.hh>

///
/// these includes are placed here in order to prevent pre-processing
/// conflicts.
///
#include <elle/miscellaneous/Report.hh>
#include <elle/miscellaneous/Maid.hh>

namespace elle
{
  namespace core
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this defines the maximum number of frames included in a trace.
    ///
    const Natural32			Trace::Size = 30;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Trace::Trace(const Void*					address):
      address(address)
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
      void*		addresses[Trace::Size];
      char**		symbols;
      Natural32		size;
      Natural32		i;

      enter();

      // retrieve the frame addresses.
      size = ::backtrace(addresses, Trace::Size);

      // transform the addresses into human-kind-of-readable strings.
      if ((symbols = ::backtrace_symbols(addresses, size)) == NULL)
	return;

      // go through the symbols, starting with 1 in order to
      // ignore the current method which is irrelevant to the trace.
      for (i = 1; i < size; i++)
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
	      // record the symbol, as such.
	      this->symbols.push_back(line);

	      continue;
	    }

	  // set the first index as following the parenthese.
	  first = index;

	  // find the + character, indicating the end of the mangled name.
	  if ((index = line.find_first_of("+", first)) == String::npos)
	    {
	      // record the symbol, as such.
	      this->symbols.push_back(line);

	      continue;
	    }

	  // set the last index as precedin the + character.
	  last = index;

	  // ignore if the name is empty.
	  if ((last - 1) == first)
	    {
	      // record the symbol, as such.
	      this->symbols.push_back(line);

	      continue;
	    }

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
		// an error occured.
		return;

		break;
	      }
	    case -2:
	      {
		// record the symbol, as such.
		this->symbols.push_back(line);

		continue;
	      }
	    }

	  // record the demangled symbol.
	  this->symbols.push_back(symbol);

	  // release the symbol's memory.
	  ::free(symbol);
	}

      // the symbols can now be released.
      ::free(symbols);
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
      Trace::Scoutor	scoutor;
      Natural32		i;

      enter();

      std::cout << alignment << "[Trace]" << std::endl;

      // dump the address.
      std::cout << alignment << Dumpable::Shift << "[Address] "
		<< this->address << std::endl;

      std::cout << alignment << Dumpable::Shift << "[Symbols] " << std::endl;

      // print all the symbols.
      for (scoutor = this->symbols.begin();
	   scoutor != this->symbols.end();
	   scoutor++)
	{
	  std::cout << alignment << Dumpable::Shift << Dumpable::Shift
		    << *scoutor << std::endl;
	}

      leave();
    }

  }
}
