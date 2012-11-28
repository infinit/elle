#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include <elle/log.hh>
#include <elle/Exception.hh>

#include "Array.hh"
#include "Bool.hh"
#include "Dictionary.hh"
#include "Float.hh"
#include "Integer.hh"
#include "Parser.hh"
#include "String.hh"

ELLE_LOG_COMPONENT("elle.format.json.Parser");

namespace elle
{
  namespace format
  {
    namespace json
    {

      template class Parser<std::string>;
      template<> std::string const Parser<std::string>::_whitespaces = " \r\n\t";
      template<> std::string const Parser<std::string>::_nullString  = "null";
      template<> std::string const Parser<std::string>::_trueString  = "true";
      template<> std::string const Parser<std::string>::_falseString = "false";

      template<typename CharType>
      std::unique_ptr<Object>
      parse(std::basic_istream<CharType>& in,
            ParserOption flags)
      {
        return Parser<std::basic_string<CharType>>(flags).parse(in);
      }

      template <typename CharType>
      std::unique_ptr<Object>
      parse(std::basic_string<CharType> const& in,
            ParserOption flags)
      {
        std::stringstream ss(in);
        return parse(ss, flags);
      }

      template <typename T>
      class Parser<T>::Error:
          public std::runtime_error
      {
      public:
        Error(StreamType& in,
              std::string const& error):
          std::runtime_error(error + ": " + _getString(in))
        {}
      private:
        static
        std::string
        _getString(StreamType& in)
        {
          CharType str[256];
          in.clear();
          in.read(&str[0], 256);

          std::string res(str, 256);
#include <elle/idiom/Close.hh>
          return "Stream(state: " +
            std::string(in.eof() ? "EOF" : (in.fail() ? "FAIL" : "GOOD")) +
            "): '" + res + "'";
#include <elle/idiom/Open.hh>
        }
      };

      template<typename T>
      typename Parser<T>::ObjectPtr
      Parser<T>::parse(StreamType& input)
      {
        ELLE_TRACE_SCOPE("Parsing json from stream");
        if (!input.good())
          throw elle::Exception("The stream is not good.");
        if (input.eof())
          throw elle::Exception("The stream is empty.");

        ObjectPtr res;
        if (!_ReadJSONValue(input, res))
          throw elle::Exception("Couldn't read any JSON value");
        assert(res && "true returned, the object should not be null");
        return res;
      }

      template<typename T>
      bool
      Parser<T>::_ReadJSONValue(StreamType& in,
                                ObjectPtr& out)
      {
        ELLE_TRACE_SCOPE("Trying reading any value.");
        _Skip(in);
        return (
            _ReadJSONInt(in, out)      ||
            _ReadJSONFloat(in, out)    ||
            _ReadJSONString(in, out)   ||
            _ReadJSONDict(in, out)     ||
            _ReadJSONArray(in, out)    ||
            _ReadJSONBool(in, out)     ||
            _ReadJSONNull(in, out)
        );
      }

      template<typename T>
      bool
      Parser<T>::_ReadJSONBool(StreamType& in,
                               ObjectPtr& out)
      {
        ELLE_TRACE_SCOPE("Trying reading bool.");
        if (_ReadString(in, _trueString))
          out.reset(new json::Bool(true));
        else if (_ReadString(in, _falseString))
          out.reset(new json::Bool(false));
        else
          return false;
        return true;
      }

      template<typename T>
      bool
      Parser<T>::_ReadJSONNull(StreamType& in,
                               ObjectPtr& out)
      {
        ELLE_TRACE_SCOPE("Trying reading null.");
        if (_ReadString(in, _nullString))
          {
            out.reset(new json::Null);
            return true;
          }
        return false;
      }

      template<typename T>
      bool
      Parser<T>::_ReadJSONInt(StreamType& in,
                              ObjectPtr& out)
      {
        ELLE_TRACE_SCOPE("Trying reading int.");
        assert(in.good());
        auto pos = in.tellg();
        assert(pos != -1);
        json::Integer::Type i;

        in >> i;
        CharType c = 0;
        if (!in.eof() && in.good())
          {
            auto charpos = in.tellg();
            assert(pos != -1 && "Corrupition");
            in >> c;
            if (c != '.')
              in.seekg(charpos);
          }
#include <elle/idiom/Close.hh>
        if (in.fail() || c == '.')
#include <elle/idiom/Open.hh>
          {
            in.clear();
            in.seekg(pos);
            assert(in.good());
            return false;
          }
        out.reset(new json::Integer(i));
        return true;
      }

      template<typename T>
      bool
      Parser<T>::_ReadJSONFloat(StreamType& in,
                                ObjectPtr& out)
      {
        ELLE_TRACE_SCOPE("Trying reading float.");
        auto pos = in.tellg();
        json::Float::Type f;
        in >> f;
#include <elle/idiom/Close.hh>
        if (in.fail())
#include <elle/idiom/Open.hh>
          {
            in.clear();
            in.seekg(pos);
            assert(in.good());
            return false;
          }
        out.reset(new json::Float(f));
        return true;
      }

      template<typename T>
      bool
      Parser<T>::_ReadJSONString(StreamType& in,
                                 ObjectPtr& out)
      {
        ELLE_TRACE_SCOPE("Trying reading string.");
        std::string res;

        if (!_ReadChar(in, '"'))
          return false;

        auto pos = in.tellg();
        while (in.good())
          {
            CharType c = in.get();
            if (c == '\\')
              {
                if (!in.good())
                  break;
                c = in.get();
                switch (c)
                {
                case 'n':   res.push_back('\n');  break;
                case 't':   res.push_back('\t');  break;
                case 'r':   res.push_back('\r');  break;
                default:    res.push_back(c);     break;
                }
              }
            else if (c == '"')
              {
                out.reset(new json::String(res));
                return true;
              }
            else
              res.push_back(c);
          }
        in.seekg(pos);

        return false;
      }

      template<typename T>
      bool
      Parser<T>::_ReadJSONArray(StreamType& in,
                                ObjectPtr& out)
      {
        ELLE_TRACE_SCOPE("Trying reading array.");
        if (!_ReadChar(in, '['))
          return false;

        std::unique_ptr<json::Array> res(new json::Array);

        auto pos = in.tellg();
        while (in.good())
          {
            _Skip(in);
            std::unique_ptr<Object> value;
            if (!_ReadJSONValue(in, value))
              {
                if (res->size() > 0)
                  throw Error(in, "Expected value following a ',' in an array");
              }
            else
              {
                res->push_back(std::move(value));

                _Skip(in);
                if (_ReadChar(in, ','))
                  {
                    continue;
                  }
              }

            if (_ReadChar(in, ']'))
              {
                out.reset(res.release());
                return true;
              }
            throw Error(in, "Expected ',' or ']' in an array");
          }
        in.seekg(pos);
        return false;
      }

      template<typename T>
      bool
      Parser<T>::_ReadJSONDict(StreamType& in,
                               ObjectPtr& out)
      {
        ELLE_TRACE_SCOPE("Trying reading dict.");
        if (!_ReadChar(in, '{'))
          return false;
        std::unique_ptr<json::Dictionary> res(new json::Dictionary);

        ELLE_DEBUG_SCOPE("Start reading dict.");
        while (in.good())
          {
            _Skip(in);

            std::unique_ptr<Object> key;
            if (!_ReadJSONString(in, key))
              {
                if (res->size() > 0)
                  throw Error(in, "Could not read a dictionary key");
              }
            else
              {
                assert(dynamic_cast<json::String*>(key.get()) != nullptr);

                _Skip(in);

                if (!_ReadChar(in, ':'))
                  throw Error(in, "':' expected");

                _Skip(in);

                std::unique_ptr<Object> value;
                if (!_ReadJSONValue(in, value))
                  throw Error(in, "Could not read dictionary pair");

                (*res)[static_cast<json::String&>(*key)] = std::move(value);

                _Skip(in);
                if (_ReadChar(in, ','))
                  continue;
              }

            if (_ReadChar(in, '}'))
              {
                out.reset(res.release());
                return true;
              }
            throw Error(in, "Expected ',' or '}' after a dictionary value");
          }
        throw Error(in, "Dictionnary was bad formed.");
      }


      template<typename T>
      bool
      Parser<T>::_ReadChar(StreamType& in,
                           CharType val)
      {
        if (!in.eof() && in.good())
          {
            auto pos = in.tellg();
            CharType c;
            in >> c;
            if (c == val)
              {
                ELLE_DEBUG("read : %c", c);
                return true;
              }
            ELLE_DEBUG("Expected char %c but found %c.", val, c);
            in.seekg(pos);
          }
        else
        {
          ELLE_DEBUG("Stream is not valid.");
        }
        return false;
      }


      template<typename T>
      bool
      Parser<T>::_ReadString(StreamType& in,
                             StringType const& val)
      {
        auto pos = in.tellg();
        auto it = val.begin(),
             end = val.end();
        for (; it != end; ++it)
          {
            if (!_ReadChar(in, *it))
              {
                in.seekg(pos);
                return false;
              }
          }
        return true;
      }


      template<typename T>
      void
      Parser<T>::_Skip(StreamType& in,
                       StringType const& chars)
      {
        while (!in.eof() && in.good())
          {
            CharType c = in.peek();
            if (chars.find(c) == StringType::npos)
              break;
            in.ignore(256, c);
          }
      }

      template
      std::unique_ptr<Object>
      parse(std::basic_istream<std::string::value_type>& in,
            ParserOption flags);

      template
      std::unique_ptr<Object>
      parse(std::basic_string<std::string::value_type> const& in,
            ParserOption flags);

      //template class Parser<std::wstring>;
      //template<> std::wstring const Parser<std::wstring>::_whitespaces = L" \r\n\t";
      //template<> std::wstring const Parser<std::wstring>::_nullString  = L"null";
      //template<> std::wstring const Parser<std::wstring>::_trueString  = L"true";
      //template<> std::wstring const Parser<std::wstring>::_falseString = L"false";
      //template
      //std::unique_ptr<Object>
      //parse(std::basic_istream<std::string::value_type>& in,
      //      ParserOption flags);

    }
  }
} // !namespace elle::format::json
