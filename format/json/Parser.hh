#ifndef ELLE_FORMAT_JSON_PARSER_HH
# define ELLE_FORMAT_JSON_PARSER_HH

# include <stdexcept>
# include <string>

# include "Object.hh"

namespace elle
{
  namespace format
  {
    namespace json
    {

      /// Parser behavior may be influenced with these flags.
      enum class ParserOption : int
      {
        None = 0x00,
      };

      /// Convenient method to construct a json object from stream.
      ///
      /// It will construct a parser with the given options and parse the
      /// stream.
      /// @throws Parser::Error.
      template<typename CharType = std::string::value_type>
      std::unique_ptr<Object>
      parse(std::basic_istream<CharType>& in,
            ParserOption flags = ParserOption::None);

      template <typename StringType_ = std::string>
      class Parser
      {
      private:
        typedef StringType_                       StringType;
        typedef typename StringType::value_type   CharType;
        typedef std::basic_istream<CharType>      StreamType;
        typedef std::unique_ptr<json::Object>     ObjectPtr;
      public:
        class Error;

      private:
        static StringType const _whitespaces;
        static StringType const _trueString;
        static StringType const _falseString;
        static StringType const _nullString;

      private:
        ParserOption _options;

      public:
        Parser(ParserOption options = ParserOption::None):
          _options(options)
        {}
        ObjectPtr
        parse(StreamType& input);

      private:
        // XXX internal methods should be lower cased
        bool _ReadJSONValue(StreamType& in,
                            ObjectPtr& out);
        bool _ReadJSONBool(StreamType& in,
                           ObjectPtr& out);
        bool _ReadJSONNull(StreamType& in,
                           ObjectPtr& out);
        bool _ReadJSONInt(StreamType& in,
                          ObjectPtr& out);
        bool _ReadJSONFloat(StreamType& in,
                            ObjectPtr& out);
        bool _ReadJSONString(StreamType& in,
                             ObjectPtr& out);
        bool _ReadJSONArray(StreamType& in,
                            ObjectPtr& out);
        bool _ReadJSONDict(StreamType& in,
                           ObjectPtr& out);
        bool _ReadChar(StreamType& in,
                       CharType val);
        bool _ReadString(StreamType& in,
                         StringType const& val);
        void _Skip(StreamType& in,
                   StringType const& chars = _whitespaces);
      };

    }
  }
} // !namespace elle::format::json

#endif /* ! PARSER_HH */
