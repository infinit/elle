//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sun jun 26 21:23:03 2011]
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
      static const Byte         Value = Parser::TypeUnknown;
    };

    ///
    /// this macro-function link the type to the enum value in a simple
    /// call.
    ///
#define ParserDeclare(_type_)                                           \
  template <>                                                           \
  struct ParserType<_type_>                                             \
  {                                                                     \
    static const Byte           Value = Parser::Type ## _type_; \
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
    Status      Parser::Behaviour<T, C>::Value(Parser&          parser,
                                               const String&    name,
                                               T&               value)
    {
      Parser::Option*   option;

      enter();

      // locate the option.
      if (parser.Locate(name, option) == StatusFalse)
        escape("unable to locate the option '%s'",
               name.c_str());

      // if the option has not been activated, return an error.
      if (option->state == Parser::StateDeactivated)
        escape("the option '%s' has not been activated",
               name.c_str());

      // if no argument has been provided, return an error.
      if (option->value == NULL)
        escape("the option '%s' has not been provided with an argument",
               name.c_str());

      // convert the string-based argument to the given type, if possible.
      if (Variable::Convert(*option->value, value) == StatusFalse)
        escape("unable to convert the argument '%s' for the option '%s'",
               option->value->c_str(), name.c_str());

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
    Status      Parser::Behaviour<T, C>::Value(Parser&          parser,
                                               const String&    name,
                                               T&               value,
                                               const T          D)
    {
      Parser::Option*   option;

      enter();

      // locate the option.
      if ((parser.Locate(name, option) == StatusTrue) &&
          (option->value != NULL))
        {
          // convert the string-based argument to the given type, if possible.
          if (Variable::Convert(*option->value, value) == StatusFalse)
            escape("unable to convert the argument '%s' for the option '%s'",
                   option->value->c_str(), name.c_str());
        }
      else
        {
          // set the default value.
          value = D;
        }

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
    Status      Parser::Behaviour<T, true>::Value(
                  Parser&                                       parser,
                  const String&                                 name,
                  Uniquable<F>&                                 object)
    {
      Parser::Option*   option;

      enter();

      // locate the option.
      if (parser.Locate(name, option) == StatusFalse)
        escape("unable to locate the option '%s'",
               name.c_str());

      // if the option has not been activated, return an error.
      if (option->state == Parser::StateDeactivated)
        escape("the option '%s' has not been activated",
               name.c_str());

      // if no argument has been provided, return an error.
      if (option->value == NULL)
        escape("the option '%s' has not been provided with an argument",
               name.c_str());

      // restore the object.
      if (object.Restore(*option->value) == StatusError)
        escape("unable to restore the object '%s' for the option '%s'",
               option->value->c_str(), name.c_str());

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
    Status      Parser::Behaviour<T, true>::Value(
                  Parser&                                       parser,
                  const String&                                 name,
                  Uniquable<F>&                                 object,
                  const Uniquable<F>                            D)
    {
      Parser::Option*   option;

      enter();

      // locate the option.
      if ((parser.Locate(name, option) == StatusTrue) &&
          (option->value != NULL))
        {
          // restore the object.
          if (object.Restore(*option->value) == StatusError)
            escape("unable to restore the object '%s' for the option '%s'",
                   option->value->c_str(), name.c_str());
        }
      else
        {
          Unique        unique;

          // save the default value.
          if (D.Save(unique) == StatusError)
            escape("unable to save the default value for the option '%s'",
                   name.c_str());

          // restore the object.
          if (object.Restore(unique) == StatusError)
            escape("unable to restore the object '%s' for the option '%s'",
                   unique.c_str(), name.c_str());
        }

      leave();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method wraps the Value() method by forwarding the call
    /// according to the given type.
    ///
    template <typename T>
    Status              Parser::Value(const String&             name,
                                      T&                        value)
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
    /// this method wraps the default-based Value() method by forwarding
    /// the call according to the given type.
    ///
    template <typename T>
    Status              Parser::Value(const String&             name,
                                      T&                        value,
                                      const T&                  D)
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
