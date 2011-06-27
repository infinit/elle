//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/utility/Parser.hxx
//
// created       julien quintard   [sun jun 26 21:23:03 2011]
// updated       julien quintard   [mon jun 27 22:20:04 2011]
//

#ifndef ELLE_UTILITY_PARSER_HXX
#define ELLE_UTILITY_PARSER_HXX

namespace elle
{
  namespace utility
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// this base template allows for specialized-template.
    ///
    template <typename T>
    struct ParserType
    {
      static const Byte		Value = Parser::TypeUnknown;
    };

    ///
    /// this macro-function link the type to the enum value in a simple
    /// call.
    ///
#define ParserDeclare(_type_)						\
  template <>								\
  struct ParserType<_type_>						\
  {									\
    static const Byte		Value = Parser::Type ## _type_;	\
  };

    ///
    /// these macro-function calls actually generate the specialized-templates
    /// for every basic type of the elle library.
    ///
    ParserDeclare(Null);
    ParserDeclare(Boolean);
    ParserDeclare(Character);
    ParserDeclare(Real);
    ParserDeclare(Integer8);
    ParserDeclare(Integer16);
    ParserDeclare(Integer32);
    ParserDeclare(Integer64);
    ParserDeclare(Natural8);
    ParserDeclare(Natural16);
    ParserDeclare(Natural32);
    ParserDeclare(Natural64);
    ParserDeclare(String);

//
// ---------- behaviours ------------------------------------------------------
//

    ///
    /// this method retrieves a basic type argument associated with the given
    /// option.
    ///
    /// note however that if the option has not been activated or provided
    /// with an argument, the method returns an error.
    ///
    template <typename T, Boolean C>
    Status	Parser::Behaviour<T, C>::Value(Parser&		parser,
					       const String&	name,
					       T&		value)
    {
      Parser::Option*	option;

      enter();

      // locate the option.
      if (parser.Locate(name, option) == StatusFalse)
	escape("unable to locate the option");

      // if the option has not been activated, return an error.
      if (option->state == Parser::StateDeactivated)
	/// XXX \todo precise the short/long option name here
	escape("the option has not been activated");

      // if no argument has been provided, return an error.
      if (option->value == NULL)
	/// XXX \todo precise the short/long option name here
	escape("the option has not been provided with an argument");

      // convert the string-based argument to the given type, if possible.
      if (Variable::Convert(*option->value, value) == StatusError)
	escape("unable to convert the argument");

      leave();
    }

    ///
    /// this method tries to retrieve the argument associated with the
    /// given option.
    ///
    /// however, should have the option not been activated or not provided
    /// with an argument, the method still returns properly by taking care
    /// to assign the default value.
    ///
    template <typename T, Boolean C>
    Status	Parser::Behaviour<T, C>::Value(Parser&		parser,
					       const String&	name,
					       T&		value,
					       const T&		D)
    {
      Parser::Option*	option;

      enter();

      // locate the option.
      if (parser.Locate(name, option) == StatusFalse)
	escape("unable to locate the option");

      // if no argument has been provided, return the default value.
      if (option->value == NULL)
	{
	  // set the default value.
	  value = D;

	  leave();
	}

      // convert the string-based argument to the given type, if possible.
      if (Variable::Convert(*option->value, value) == StatusError)
	escape("unable to convert the argument");

      leave();
    }

    ///
    /// this method retrieves a compound argument.
    ///
    /// note however that if the option has not been activated or provided
    /// with an argument, the method returns an error.
    ///
    template <typename T>
    template <const Format F>
    Status	Parser::Behaviour<T, true>::Value(
		  Parser&					parser,
		  const String&					name,
		  Uniquable<F>&					object)
    {
      Parser::Option*	option;

      enter();

      // locate the option.
      if (parser.Locate(name, option) == StatusFalse)
	escape("unable to locate the option");

      // if the option has not been activated, return an error.
      if (option->state == Parser::StateDeactivated)
	/// XXX \todo precise the short/long option name here
	escape("the option has not been activated");

      // if no argument has been provided, return an error.
      if (option->value == NULL)
	/// XXX \todo precise the short/long option name here
	escape("the option has not been provided with an argument");

      // restore the object.
      if (object.Restore(*option->value) == StatusError)
	escape("unable to restore the object");

      leave();
    }

    ///
    /// this method retrieves a compound argument, with a potential
    /// default value.
    ///
    /// however, should have the option not been activated or not provided
    /// with an argument, the method still returns properly by taking care
    /// to assign the default value.
    ///
    template <typename T>
    template <const Format F>
    Status	Parser::Behaviour<T, true>::Value(
		  Parser&					parser,
		  const String&					name,
		  Uniquable<F>&					object,
		  const Uniquable<F>&				D)
    {
      Parser::Option*	option;

      enter();

      // locate the option.
      if (parser.Locate(name, option) == StatusFalse)
	escape("unable to locate the option");

      // if no argument has been provided, return the default value.
      if (option->value == NULL)
	{
	  Unique	unique;

	  // save the default value.
	  if (D.Save(unique) == StatusError)
	    escape("unable to save the default value");

	  // restore the object.
	  if (object.Restore(unique) == StatusError)
	    escape("unable to restore the object");

	  leave();
	}

      // restore the object.
      if (object.Restore(*option->value) == StatusError)
	escape("unable to restore the object");

      leave();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename T>
    Status		Parser::Value(const String&		name,
				      T&			value)
    {
      return
	(Parser::Behaviour<T,
			   ParserType<T>::Value
			     ==
			   Parser::TypeUnknown>::Value(*this,
						       name,
						       value));
    }

    ///
    /// XXX
    ///
    template <typename T>
    Status		Parser::Value(const String&		name,
				      T&			value,
				      const T&			D)
    {
      return
	(Parser::Behaviour<T,
			   ParserType<T>::Value
			     ==
			   Parser::TypeUnknown>::Value(*this,
						       name,
						       value,
						       D));
    }

  }
}

#endif
