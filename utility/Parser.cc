//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/utility/Parser.cc
//
// created       julien quintard   [wed apr 28 11:25:27 2010]
// updated       julien quintard   [mon apr 25 11:05:38 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/utility/Parser.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  namespace utility
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable contains the alignment where the descriptions
    /// must be displayed in Usage().
    ///
    const Natural32		Parser::Alignment = 25;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Parser::Parser(Natural32					argc,
		   Character**					argv):
      argc(argc),
      argv(argv),
      longs(NULL)
    {
    }

    ///
    /// destructor.
    ///
    Parser::~Parser()
    {
      Natural32		i;

      // release the longs structure.
      if (this->longs != NULL)
	::free(this->longs);

      // go through the options.
      for (i = 0; i < this->options.size(); i++)
	{
	  // delete the option.
	  delete this->options[i];
	}

      // clear the container.
      this->options.clear();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method adds an option to the parser.
    ///
    Status		Parser::Register(const Character&	character,
					 const String&		string,
					 const String&		description,
					 const Type&		type)
    {
      Parser::Option*	option;

      enter(instance(option));

      // create a new option.
      option = new Parser::Option;

      // set the attributes.
      option->character = character;
      option->string = string;
      option->description = description;
      option->type = type;

      // add the option to the vector of options.
      this->options.push_back(option);

      // waive.
      waive(option);

      leave();
    }

    ///
    /// this method parses the arguments according to the registered
    /// options.
    ///
    /// this method returns true if there is still some arguments to
    /// process or false otherwise.
    ///
    Status		Parser::Parse(Character&		option)
    {
      enter();

      // check if the shorts and longs structure has been generated, if
      // not do it.
      if (this->longs == NULL)
	{
	  Natural32	i;

	  // first compute the short option string, starting with the ':'
	  // character.
	  this->shorts.append(":");

	  for (i = 0; i < this->options.size(); i++)
	    {
	      // append the short option character.
	      this->shorts.append(&this->options[i]->character, 1);

	      // append a special character(s) depending on the required
	      // arguments.
	      switch (this->options[i]->type)
		{
		case Parser::TypeNone:
		  {
		    // no option.
		    break;
		  }
		case Parser::TypeRequired:
		  {
		    // add the ':' character
		    this->shorts.append(":");

		    break;
		  }
		case Parser::TypeOptional:
		  {
		    // add the '::' character
		    this->shorts.append("::");

		    break;
		  }
		}
	    }

	  // allocate the structure.
	  if ((this->longs =
	       (struct ::option*)::malloc((this->options.size() + 1) *
					  sizeof(struct ::option))) == NULL)
	    escape("unable to allocate memory");

	  // initialize the structure with zeros, especially since the
	  // last entry must be set to zero anyway.
	  ::memset(this->longs,
		   0x0,
		   (this->options.size() + 1) * sizeof(struct ::option));

	  // build the long options.
	  for (i = 0; i < this->options.size(); i++)
	    {
	      // set the name by pointing to the string.
	      this->longs[i].name = this->options[i]->string.c_str();

	      // set the argument.
	      switch (this->options[i]->type)
		{
		case Parser::TypeNone:
		  {
		    // no option.
		    this->longs[i].has_arg = 0;

		    break;
		  }
		case Parser::TypeRequired:
		  {
		    // set the argument as being required.
		    this->longs[i].has_arg = 1;

		    break;
		  }
		case Parser::TypeOptional:
		  {
		    // set the argument as being optional.
		    this->longs[i].has_arg = 2;

		    break;
		  }
		}

	      // finally set the flag to NULL and the val to the equivalent
	      // short option.
	      this->longs[i].flag = NULL;
	      this->longs[i].val = this->options[i]->character;
	    }
	}

      // now process the argument.
      if ((option = getopt_long(this->argc, this->argv,
				this->shorts.c_str(),
				this->longs,
				NULL)) == -1)
	false();

      true();
    }

    ///
    /// this method prints the usage associated with the given parser.
    ///
    Void		Parser::Usage()
    {
      Natural32		i;

      // display the general usage.
      std::cerr << "[Usage] " << this->argv[0] << " {options...}"
		<< std::endl
		<< std::endl;

      std::cerr << "[Options]"
		<< std::endl;

      // go through the options.
      for (i = 0; i < this->options.size(); i++)
	{
	  Natural32	length;

	  // print the short and long option.
	  std::cerr << "  -" << this->options[i]->character
		    << ", "
		    << "--" << this->options[i]->string;

	  // add =ARG depending on the type or argument required.
	  switch (this->options[i]->type)
	    {
	    case Parser::TypeNone:
	      {
		// nothing to add: compute the length accordingly.
		length = 2 +
		  1 + 1 +
		  2 +
		  2 + this->options[i]->string.length();

		// align the text.
		std::cerr << String(Parser::Alignment - length, ' ');

		// display the description.
		std::cerr << this->options[i]->description << std::endl;

		break;
	      }
	    case Parser::TypeRequired:
	      {
		// add the indication that an argument is required.
		std::cerr << "=ARG";

		// compute the length.
		length = 2 +
		  1 + 1 +
		  2 +
		  2 + this->options[i]->string.length() + 4;

		// align the text.
		std::cerr << String(Parser::Alignment - length, ' ');

		// display the description.
		std::cerr << this->options[i]->description << std::endl;

		break;
	      }
	    case Parser::TypeOptional:
	      {
		// compute the length.
		length = 2 +
		  1 + 1 +
		  2 +
		  2 + this->options[i]->string.length();

		// align the text.
		std::cerr << String(Parser::Alignment - length, ' ');

		// display the description.
		std::cerr << this->options[i]->description << std::endl;

		// add the indication that an argument is optional by
		// adding a possibility: with or without an argument.
		std::cerr << "   "
			  << "--" << this->options[i]->string << "=ARG"
			  << std::endl;

		break;
	      }
	    }
	}

      std::cerr << std::endl;
    }

  }
}
