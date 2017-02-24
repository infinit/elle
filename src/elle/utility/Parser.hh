#ifndef ELLE_UTILITY_PARSER_HH
# define ELLE_UTILITY_PARSER_HH

#include <elle/types.hh>

#include <elle/Exception.hh>

# include <iostream>
# include <vector>
# include <getopt.h>

namespace elle
{
  namespace utility
  {
    /*
     * This class is used to throw exception from the Parser, instead of using
     * escape.
     *
     * This allow for clearer diagnostics of incorrect inputs.
     */
    class ParserException : public elle::Exception
    {
    public:
      template <typename ...ARGS>
      ParserException(elle::String const &fmt, ARGS && ... args);
    };

    ///
    /// this class wraps the command line parser by enabling one to define
    /// options but also to retrieve the passed arguments directly in
    /// their type by operating an automatic conversion.
    ///
    class Parser
    {
    public:
      //
      // constants
      //
      static const Natural32            Alignment;

      //
      // enumeration
      //
      ///
      /// this enumeration types every basic element settings are composed of.
      ///
      enum Type
        {
          TypeUnknown = 0,
          TypeNull,
          Typebool,
          TypeCharacter,
          TypeReal,
          TypeInteger8,
          TypeInteger16,
          TypeInteger32,
          TypeInteger64,
          TypeNatural8,
          TypeNatural16,
          TypeNatural32,
          TypeNatural64,
          TypeString
        };

      enum Kind
        {
          KindNone,
          KindOptional,
          KindRequired
        };

      enum State
        {
          StateDeactivated,
          StateActivated
        };

      //
      // classes
      //
      class Option
      {
      public:
        //
        // constructors & destructors
        //
        Option(const String&,
               const Character&,
               const String&,
               const String&,
               const Kind&);

        //
        // attributes
        //
        String          name;

        Character       character;
        String          string;
        String          description;
        Kind            kind;

        State           state;
        String*         value;

        //
        // interfaces
        //

        // dumpable
        Status          Dump(const Natural32 = 0) const;
      };

      //
      // types
      //
      struct O
      {
        typedef std::vector<Option*>            Container;
        typedef Container::iterator             Iterator;
        typedef Container::const_iterator       Scoutor;
      };

      struct E
      {
        typedef std::vector<elle::String>       Container;
        typedef Container::iterator             Iterator;
        typedef Container::const_iterator       Scoutor;
      };

      //
      // constructors & destructors
      //
      Parser(Natural32,
             Character**);
      ~Parser();

      //
      // methods
      //
      Status            Description(const String&);

      Status            Register(const String&,
                                 const Character&,
                                 const String&,
                                 const String&,
                                 const Kind);

      Status            Example(const String&);

      bool Locate(const String&,
                               Option*&);
      bool Locate(const Character&,
                               Option*&);

      Status            Parse();

      bool Test(const String&);
      bool Argument(const String&);

      template <typename T>
      Status            Value(const String&,
                              T&);
      template <typename T>
      Status            Value(const String&,
                              T&,
                              const T&);

      //
      // behaviours
      //
      template <typename T, bool C>
      struct            Behaviour
      {
        static Status   Value(Parser&,
                              const String&,
                              T&);
        static Status   Value(Parser&,
                              const String&,
                              T&,
                              const T);
      };

      Void              Usage();

      //
      // interfaces
      //

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Natural32         argc;
      Character**       argv;

      String            description;

      O::Container      options;
      E::Container      examples;

      String            shorts;
      struct ::option*  longs;
    };

  }
}

#include <elle/utility/Parser.hxx>

#endif
