#ifndef ELLE_UTILITY_PARSER_HXX
# define ELLE_UTILITY_PARSER_HXX

# include <elle/Exception.hh>

# include <elle/idiom/Close.hh>
#  include <boost/lexical_cast.hpp>
# include <elle/idiom/Open.hh>

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
      Parser::Option*   option;

      if (this->Locate(name, option) == Status::False)
          escape("unable to locate the option '%s'", name.c_str());

      if (option->state == Parser::StateDeactivated)
        escape("the option '%s' has not been activated",
               name.c_str());

      // if no argument has been provided, return an error.
      if (option->value == nullptr)
        escape("the option '%s' has not been provided with an argument",
               name.c_str());

      try
        {
          value = boost::lexical_cast<T>(*option->value);
        }
      catch (std::exception const& err)
        {
          escape(
            "unable to convert the argument '%s' for the option '%s'",
            option->value->c_str(), name.c_str()
          );
        }
      return elle::Status::Ok;
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
      Option* option;
      if (this->Locate(name, option) == Status::False || option->value == nullptr)
        {
          value = D;
          return elle::Status::Ok;
        }

      if (option->state == Parser::StateDeactivated)
        escape("the option '%s' has not been activated", name.c_str());

      // if no argument has been provided, return an error.
      if (option->value == nullptr)
        escape("the option '%s' has not been provided with an argument",
               name.c_str());

      try
        {
          value = boost::lexical_cast<T>(*option->value);
        }
      catch (std::exception const& err)
        {
          escape(
            "unable to convert the argument '%s' for the option '%s'",
            option->value->c_str(), name.c_str()
          );
        }
      return elle::Status::Ok;
    }

  }
}

#endif
