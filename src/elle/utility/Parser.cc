#include <elle/utility/Parser.hh>
#include <elle/io/Dumpable.hh>

#include <unistd.h>

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
    const uint32_t             Parser::Alignment = 25;

//
// ---------- option-specific methods -----------------------------------------
//

    ///
    /// default constructor.
    ///
    Parser::Option::Option(const String&                        name,
                           const Character&                     character,
                           const String&                        string,
                           const String&                        description,
                           const Kind&                          kind):
      name(name),
      character(character),
      string(string),
      description(description),
      kind(kind),

      state(Parser::StateDeactivated),
      value(nullptr)
    {
    }

    ///
    /// this method dumps the option.
    ///
    Status              Parser::Option::Dump(const uint32_t    margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Option] " << this->name << std::endl;

      // dump the character.
      std::cout << alignment << io::Dumpable::Shift
                << "[Character] " << this->character << std::endl;

      // dump the string.
      std::cout << alignment << io::Dumpable::Shift
                << "[String] " << this->string << std::endl;

      // dump the description.
      std::cout << alignment << io::Dumpable::Shift
                << "[Description] " << this->description << std::endl;

      // dump the kind.
      std::cout << alignment << io::Dumpable::Shift
                << "[Kind] " << this->kind << std::endl;

      // dump the state.
      std::cout << alignment << io::Dumpable::Shift
                << "[State] " << this->state << std::endl;

      // dump the value, if present.
      if (this->value != nullptr)
        {
          std::cout << alignment << io::Dumpable::Shift
                    << "[Value] " << *this->value << std::endl;
        }
      else
        {
          std::cout << alignment << io::Dumpable::Shift
                    << "[Value] " << "none" << std::endl;
        }

      return Status::Ok;
    }

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Parser::Parser(uint32_t                                    argc,
                   Character**                                  argv):
      argc(argc),
      argv(argv),
      longs(nullptr)
    {
    }

    ///
    /// destructor.
    ///
    Parser::~Parser()
    {
      uint32_t         i;

      // release the longs structure.
      if (this->longs != nullptr)
        ::free(this->longs);

      // go through the options.
      for (i = 0; i < this->options.size(); i++)
        {
          // delete the value, if present.
          if (this->options[i]->value != nullptr)
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
    Status              Parser::Description(const String&       description)
    {
      // set the description.
      this->description = description;

      return Status::Ok;
    }

    ///
    /// this method adds an option to the parser.
    ///
    Status              Parser::Register(const String&          name,
                                         const Character&       character,
                                         const String&          string,
                                         const String&          description,
                                         const Kind             kind)
    {
      Parser::Option*   option;

      // create a new option.
      option = new Parser::Option(name,
                                  character,
                                  string,
                                  description,
                                  kind);

      // add the option to the vector of options.
      this->options.push_back(option);

      return Status::Ok;
    }

    ///
    /// this method adds an example to the parser.
    ///
    Status              Parser::Example(const String&           example)
    {
      // add the example.
      this->examples.push_back(example);

      return Status::Ok;
    }

    ///
    /// this method returns true and sets the pointer if the given
    /// named option has been located.
    ///
    bool Parser::Locate(const String&            name,
                                       Parser::Option*&         option)
    {
      uint32_t         i;

      // go through the options.
      for (i = 0; i < this->options.size(); i++)
        {
          // is this the right option.
          if (this->options[i]->name == name)
            {
              // set the pointer.
              option = this->options[i];

              return true;
            }
        }

      return false;
    }

    ///
    /// this method returns true and sets the pointer if the given
    /// character option has been located.
    ///
    bool Parser::Locate(const Character&         character,
                                       Parser::Option*&         option)
    {
      uint32_t         i;

      // go through the options.
      for (i = 0; i < this->options.size(); i++)
        {
          // is this the right option.
          if (this->options[i]->character == character)
            {
              // set the pointer.
              option = this->options[i];

              return true;
            }
        }

      return false;
    }

    ///
    /// this method parses the arguments according to the registered
    /// options.
    ///
    /// this method returns true if there is still some arguments to
    /// process or false otherwise.
    ///
    Status              Parser::Parse()
    {
      Character         character;

      // check if the shorts and longs structure has been generated, if
      // not do it.
      if (this->longs == nullptr)
        {
          uint32_t     i;

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
                 static_cast<struct ::option*>(
                   ::malloc((this->options.size() + 1) *
                            sizeof (struct ::option)))) == nullptr)
            throw Exception("unable to allocate memory");

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
                    this->longs[i].has_arg = no_argument;

                    break;
                  }
                case Parser::KindRequired:
                  {
                    // set the argument as being required.
                    this->longs[i].has_arg = required_argument;

                    break;
                  }
                case Parser::KindOptional:
                  {
                    // set the argument as being optional.
                    this->longs[i].has_arg = optional_argument;

                    break;
                  }
                }

              // finally set the flag to nullptr and the val to the equivalent
              // short option.
              this->longs[i].flag = nullptr;
              this->longs[i].val = this->options[i]->character;
            }
        }

      // now process the argument.
      while ((character =
              static_cast<Character>(getopt_long(this->argc,
                                                 this->argv,
                                                 this->shorts.c_str(),
                                                 this->longs,
                                                 nullptr))) != -1)
        {
          Parser::Option*       option;

          // unknown option.
          if (character == '?')
            {
              // display the usage.
              this->Usage();

              throw Exception("unknown option");
            }

          // missing argument.
          if (character == ':')
            {
              // display the usage.
              this->Usage();

              throw Exception("missing argument");
            }

          // locate the option.
          if (this->Locate(character, option) == false)
            throw Exception("unable to locate the option");

          // activate the option.
          option->state = Parser::StateActivated;

          // depending on the kind.
          switch (option->kind)
            {
            case Parser::KindNone:
              {
                // if an argument is present, return an error.
                if (optarg != nullptr)
                  throw Exception("this option is not supposed to take an argument");

                break;
              }
            case Parser::KindOptional:
              {
                // allocate and set the value, if present.
                if (optarg != nullptr)
                  option->value = new String(::optarg);

                break;
              }
            case Parser::KindRequired:
              {
                // if no argument is provided, return an error.
                if (optarg == nullptr)
                  throw Exception("this option is supposed to take an argument");

                // allocate and set the value
                option->value = new String(::optarg);

                break;
              }
            }
        }

      return Status::Ok;
    }

    ///
    /// this method returns true if the given option has been provided
    /// on the command line.
    ///
    bool Parser::Test(const String&              name)
    {
      Parser::Option*   option;

      // locate the option.
      if (this->Locate(name, option) == false)
        return false;

      // return true if the option has been activated.
      if (option->state == Parser::StateActivated)
        return true;

      return false;
    }

    ///
    /// this method returns true if the given option has been provided
    /// with an argument.
    ///
    bool Parser::Argument(const String&          name)
    {
      Parser::Option*   option;

      // locate the option.
      if (this->Locate(name, option) == false)
        return false;

      // return true if the option has been activated.
      if (option->state == Parser::StateDeactivated)
        return false;

      // return true if an argument has been provided.
      if (option->value != nullptr)
        return true;

      return false;
    }

    ///
    /// this method prints the usage associated with the given parser.
    ///
    Void                Parser::Usage()
    {
      uint32_t         i;

      // display the general usage.
      std::cerr << "[Usage] " << this->argv[0] << " {options...}"
                << std::endl
                << std::endl;

      std::cerr << "[Options]"
                << std::endl;

      // go through the options.
      for (i = 0; i < this->options.size(); i++)
        {
          uint32_t     length;

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

      // display the examples, if present.
      if (this->examples.size() > 0)
        {
          std::cerr << std::endl
                    << "[Examples]"
                    << std::endl;

          // go through the examples.
          for (i = 0; i < this->examples.size(); i++)
            std::cerr << "  $> " << this->argv[0]
                      << " " << this->examples[i] << std::endl;
        }

      // display the program description.
      if (this->description.empty() == false)
        std::cerr << std::endl << this->description << std::endl;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the parser.
    ///
    Status              Parser::Dump(const uint32_t            margin) const
    {
      String            alignment(margin, ' ');
      uint32_t         i;

      std::cout << alignment << "[Parser]" << std::endl;

      // dump argc.
      std::cout << alignment << io::Dumpable::Shift
                << "[Argc] " << this->argc << std::endl;

      // dump argv.
      std::cout << alignment << io::Dumpable::Shift
                << "[Argv] " << std::hex << this->argv << std::endl;

      // dump the description.
      std::cout << alignment << io::Dumpable::Shift
                << "[Description] " << this->description << std::endl;

      // dump the short options.
      std::cout << alignment << io::Dumpable::Shift
                << "[Short] " << this->shorts << std::endl;

      // dump the long options.
      std::cout << alignment << io::Dumpable::Shift
                << "[Long] " << std::hex << this->longs << std::endl;

      // dump the options.
      std::cout << alignment << io::Dumpable::Shift
                << "[Options]" << std::endl;

      for (i = 0; i < this->options.size(); i++)
        {
          // dump the option.
          if (this->options[i]->Dump(margin + 4) == Status::Error)
            throw Exception("unable to dump the option");
        }

      // dump the examples.
      std::cout << alignment << io::Dumpable::Shift
                << "[Examples]" << std::endl;

      for (i = 0; i < this->examples.size(); i++)
        {
          // dump the example.
          std::cout << alignment << io::Dumpable::Shift << io::Dumpable::Shift
                    << this->examples[i] << std::endl;
        }

      return Status::Ok;
    }

  }
}
