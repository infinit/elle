#ifndef ELLE_FORMAT_JSON_PARSER_HH
# define ELLE_FORMAT_JSON_PARSER_HH

# include <stdexcept>
# include <string>

# include "Object.hh"

namespace elle { namespace format { namespace json {

    enum class ParserOption : int
    {
      None = 0x00,
    };

    template<typename StringType_ = std::string>
    class Parser
    {
    private:
      typedef StringType_                       StringType;
      typedef typename StringType::value_type   CharType;
      typedef std::basic_istream<CharType>      StreamType;
      typedef std::unique_ptr<json::Object>     ObjectPtr;
    public:
      class Error : public std::runtime_error
      {
      public:
        Error(StreamType& in, std::string const& error) :
          std::runtime_error(error + ": " + _getString(in))
        {}
      private:
        static std::string _getString(StreamType& in);
      };


    private:
      static StringType const _whitespaces;
      static StringType const _trueString;
      static StringType const _falseString;
      static StringType const _nullString;

    private:
      int _options;

    public:
      Parser(int options = 0) : _options(options) {}
      ObjectPtr Parse(StreamType& input);

    private:
      bool _ReadJSONValue(StreamType& in,   ObjectPtr& out);
      bool _ReadJSONBool(StreamType& in,    ObjectPtr& out);
      bool _ReadJSONNull(StreamType& in,    ObjectPtr& out);
      bool _ReadJSONInt(StreamType& in,     ObjectPtr& out);
      bool _ReadJSONFloat(StreamType& in,   ObjectPtr& out);
      bool _ReadJSONString(StreamType& in,  ObjectPtr& out);
      bool _ReadJSONArray(StreamType& in,   ObjectPtr& out);
      bool _ReadJSONDict(StreamType& in,    ObjectPtr& out);

      bool _ReadChar(StreamType& in, CharType val);
      bool _ReadString(StreamType& in, StringType const& val);
      void _Skip(StreamType& in, StringType const& chars = _whitespaces);
    };

}}} // !namespace elle::format::json

#endif /* ! PARSER_HH */


