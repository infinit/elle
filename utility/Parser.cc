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
// updated       julien quintard   [mon jul 11 22:14:55 2011]
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
// ---------- option-specific methods -----------------------------------------
//

    ///
    /// default constructor.
    ///
    Parser::Option::Option(const String&			name,
			   const Character&			character,
			   const String&			string,
			   const String&			description,
			   const Kind&				kind):
      name(name),
      character(character),
      string(string),
      description(description),
      kind(kind),

      state(Parser::StateDeactivated),
      value(NULL)
    {
    }

    ///
    /// this method dumps the option.
    ///
    Status		Parser::Option::Dump(const Natural32	margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Option] " << this->name << std::endl;

      // dump the character.
      std::cout << alignment << Dumpable::Shift
		<< "[Character] " << this->character << std::endl;

      // dump the string.
      std::cout << alignment << Dumpable::Shift
		<< "[String] " << this->string << std::endl;

      // dump the description.
      std::cout << alignment << Dumpable::Shift
		<< "[Description] " << this->description << std::endl;

      // dump the kind.
      std::cout << alignment << Dumpable::Shift
		<< "[Kind] " << (const Natural32)this->kind << std::endl;

      // dump the state.
      std::cout << alignment << Dumpable::Shift
		<< "[State] " << (const Natural32)this->state << std::endl;

      // dump the value, if present.
      if (this->value != NULL)
	{
	  std::cout << alignment << Dumpable::Shift
		    << "[Value] " << *this->value << std::endl;
	}
      else
	{
	  std::cout << alignment << Dumpable::Shift
		    << "[Value] " << none << std::endl;
	}

      leave();
    }

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
	  // delete the value, if present.
	  if (this->options[i]->value != NULL)
	    delete this->options[i]->value;

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
    /// this method adds a description to the parser.
    ///
    Status		Parser::Description(const String&	description)
    {
      enter();

      // set the description.
      this->description = description;

      leave();
    }

    ///
    /// this method adds an option to the parser.
    ///
    Status		Parser::Register(const String&		name,
					 const Character&	character,
					 const String&		string,
					 const String&		description,
					 const Kind		kind)
    {
      Parser::Option*	option;

      enter(instance(option));

      // create a new option.
      option = new Parser::Option(name,
				  character,
				  string,
				  description,
				  kind);

      // add the option to the vector of options.
      this->options.push_back(option);

      // waive.
      waive(option);

      leave();
    }

    ///
    /// this method returns true and sets the pointer if the given
    /// named option has been located.
    ///
    Status		Parser::Locate(const String&		name,
				       Parser::Option*&		option)
    {
      Natural32		i;

      enter();

      // go through the options.
      for (i = 0; i < this->options.size(); i++)
	{
	  // is this the right option.
	  if (this->options[i]->name == name)
	    {
	      // set the pointer.
	      option = this->options[i];

	      true();
	    }
	}

      false();
    }

    ///
    /// this method returns true and sets the pointer if the given
    /// character option has been located.
    ///
    Status		Parser::Locate(const Character&		character,
				       Parser::Option*&		option)
    {
      Natural32		i;

      enter();

      // go through the options.
      for (i = 0; i < this->options.size(); i++)
	{
	  // is this the right option.
	  if (this->options[i]->character == character)
	    {
	      // set the pointer.
	      option = this->options[i];

	      true();
	    }
	}

      false();
    }

    ///
    /// this method parses the arguments according to the registered
    /// options.
    ///
    /// this method returns true if there is still some arguments to
    /// process or false otherwise.
    ///
    Status		Parser::Parse()
    {
      Character		character;

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
	      switch (this->options[i]->kind)
		{
		case Parser::KindNone:
		  {
		    // no option.
		    break;
		  }
		case Parser::KindRequired:
		  {
		    // add the ':' character
		    this->shorts.append(":");

		    break;
		  }
		case Parser::KindOptional:
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
					  sizeof (struct ::option))) == NULL)
	    escape("unable to allocate memory");

	  // initialize the structure with zeros, especially since the
	  // last entry must be set to zero anyway.
	  ::memset(this->longs,
		   0x0,
		   (this->options.size() + 1) * sizeof (struct ::option));

	  // build the long options.
	  for (i = 0; i < this->options.size(); i++)
	    {
	      // set the name by pointing to the string.
	      this->longs[i].name = this->options[i]->string.c_str();

	      // set the argument.
	      switch (this->options[i]->kind)
		{
		case Parser::KindNone:
		  {
		    // no option.
		    this->longs[i].has_arg = 0;

		    break;
		  }
		case Parser::KindRequired:
		  {
		    // set the argument as being required.
		    this->longs[i].has_arg = 1;

		    break;
		  }
		case Parser::KindOptional:
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
      while ((character = getopt_long(this->argc, this->argv,
				      this->shorts.c_str(),
				      this->longs,
				      NULL)) != -1)
	{
	  Parser::Option*	option;

	  // unknown option.
	  if (character == '?')
	    {
	      // display the usage.
	      this->Usage();

	      escape("unknown option");
	    }

	  // missing argument.
	  if (character == ':')
	    {
	      // display the usage.
	      this->Usage();

	      escape("missing argument");
	    }

	  // locate the option.
	  if (this->Locate(character, option) == StatusFalse)
	    escape("unable to locate the option");

	  // activate the option.
	  option->state = Parser::StateActivated;

	  // depending on the kind.
	  switch (option->kind)
	    {
	    case Parser::KindNone:
	      {
		// if an argument is present, return an error.
		if (optarg != NULL)
		  escape("this option is not supposed to take an argument");

		break;
	      }
	    case Parser::KindOptional:
	      {
		// allocate and set the value, if present.
		if (optarg != NULL)
		  option->value = new String(::optarg);

		break;
	      }
	    case Parser::KindRequired:
	      {
		// if no argument is provided, return an error.
		if (optarg == NULL)
		  escape("this option is supposed to take an argument");

		// allocate and set the value
		option->value = new String(::optarg);

		break;
	      }
	    }
	}

      leave();
    }

    ///
    /// this method returns true if the given option has been provided
    /// on the command line.
    ///
    Status		Parser::Test(const String&		name)
    {
      Parser::Option*	option;

      enter();

      // locate the option.
      if (this->Locate(name, option) == StatusFalse)
	escape("unable to locate the option");

      // return true if the option has been activated.
      if (option->state == Parser::StateActivated)
	true();

      false();
    }

    ///
    /// this method returns true if the given option has been provided
    /// with an argument.
    ///
    Status		Parser::Argument(const String&		name)
    {
      Parser::Option*	option;

      enter();

      // locate the option.
      if (this->Locate(name, option) == StatusFalse)
	escape("unable to locate the option");

      // return true if the option has been activated.
      if (option->state == Parser::StateDeactivated)
	escape("this option has not been activated");

      // return true if an argument has been provided.
      if (option->value != NULL)
	true();

      false();
    }

    ///
    /// this method prints the usage associated with the given parser.
    ///
    Void		Parser::Usage()
    {
      Natural32		i;

      // display the program description.
      if (this->description.empty() == false)
	std::cerr << this->description << std::endl;

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

	  // add =ARG depending on the kind or argument required.
	  switch (this->options[i]->kind)
	    {
	    case Parser::KindNone:
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
	    case Parser::KindRequired:
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
	    case Parser::KindOptional:
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

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the parser.
    ///
    Status		Parser::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      Parser::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Parser]" << std::endl;

      // dump argc.
      std::cout << alignment << Dumpable::Shift
		<< "[Argc] " << this->argc << std::endl;

      // dump argv.
      std::cout << alignment << Dumpable::Shift
		<< "[Argv] " << std::hex << this->argv << std::endl;

      // dump the description.
      std::cout << alignment << Dumpable::Shift
		<< "[Description] " << this->description << std::endl;

      // dump the short options.
      std::cout << alignment << Dumpable::Shift
		<< "[Short] " << this->shorts << std::endl;

      // dump the long options.
      std::cout << alignment << Dumpable::Shift
		<< "[Long] " << std::hex << this->longs << std::endl;

      // dump the options.
      for (scoutor = this->options.begin();
	   scoutor != this->options.end();
	   scoutor++)
	{
	  Parser::Option*	option = *scoutor;

	  // dump the option.
	  if (option->Dump(margin + 2) == StatusError)
	    escape("unable to dump the option");
	}

      leave();
    }

  }
}
