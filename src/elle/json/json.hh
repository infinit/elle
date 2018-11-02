#pragma once

#include <nlohmann/json.hpp>

#include <elle/compiler.hh>

namespace elle
{
  namespace json ELLE_API
  {
    using Json = nlohmann::json;
    using Type = nlohmann::detail::value_t;

    Json
    read(std::istream& stream);

    Json
    read(std::string const& json);

    void
    write(std::ostream& stream,
          Json const& any,
          bool with_endl = true,
          bool pretty_print = false);

    std::string
    pretty_print(Json const& any);
  }
}

namespace nlohmann
{
  namespace detail
  {
    std::ostream&
    operator <<(std::ostream& s, value_t t);
  }
}

#include <elle/json/json.hxx>
