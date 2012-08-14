#include "Array.hh"
#include "Bool.hh"
#include "Dictionary.hh"
#include "Integer.hh"
#include "Factory.hh"
#include "Float.hh"
#include "Null.hh"

namespace elle
{
  namespace format
  {
    namespace json
    {

        std::unique_ptr<Null>
        Factory::construct_null()
        {
          return std::unique_ptr<Null>(new Null);
        }

        std::unique_ptr<Bool>
        Factory::construct_bool(internal::Bool value)
        {
          return std::unique_ptr<Bool>(new Bool(value));
        }

        std::unique_ptr<Float>
        Factory::construct_float(internal::Float value)
        {
          return std::unique_ptr<Float>(new Float(value));
        }

        std::unique_ptr<Integer>
        Factory::construct_integer(internal::Integer value)
        {
          return std::unique_ptr<Integer>(new Integer(value));
        }

        std::unique_ptr<String>
        Factory::construct_string(internal::String const& value)
        {
          return std::unique_ptr<String>(new String(value));
        }

        std::unique_ptr<Array>
        Factory::construct_array(internal::Array&& value)
        {
          return std::unique_ptr<Array>(new Array(std::move(value)));
        }

        std::unique_ptr<Dictionary>
        Factory::construct_dictionary(internal::Dictionary&& value)
        {
          return std::unique_ptr<Dictionary>(new Dictionary(std::move(value)));
        }

    }
  }
}

