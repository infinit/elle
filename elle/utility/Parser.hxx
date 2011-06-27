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
// updated       julien quintard   [mon jun 27 08:16:52 2011]
//

#ifndef ELLE_UTILITY_PARSER_HXX
#define ELLE_UTILITY_PARSER_HXX

namespace elle
{
  namespace utility
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method retrieves the argument associated with the given
    /// option.
    ///
    /// note however that if the option has not been activated or provided
    /// with an argument, the method returns an error.
    ///
    template <typename T>
    Status		Parser::Value(const String&		name,
				      T&			value)
    {
      Parser::Option*	option;

      enter();

      // locate the option.
      if (this->Locate(name, option) == StatusFalse)
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
    template <typename T>
    Status		Parser::Default(const String&		name,
					T&			value,
					const T			D)
    {
      Parser::Option*	option;

      enter();

      // set the default value.
      value = D;

      // locate the option.
      if (this->Locate(name, option) == StatusFalse)
	escape("unable to locate the option");

      // if no argument has been provided, return the default value.
      if (option->value == NULL)
	leave();

      // convert the string-based argument to the given type, if possible.
      if (Variable::Convert(*option->value, value) == StatusError)
	escape("unable to convert the argument");

      leave();
    }

  }
}

#endif
