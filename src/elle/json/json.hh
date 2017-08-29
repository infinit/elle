#pragma once

#include <algorithm>
#include <map>
#include <unordered_map>
#include <vector>

#include <boost/any.hpp>

#include <elle/compiler.hh>

namespace elle
{
  namespace json ELLE_API
  {
    class Json
      : public boost::any
    {
    public:
      Json() = default;
      Json(Json&& j) = default;
      Json(Json& j) = default;
      Json(Json const& j) = default;
      template <typename T>
      Json(T&& v);

      Json&
      operator =(Json const&) = default;

      Json&
      operator[] (std::string const& key);

      Json const&
      operator[] (std::string const& key) const;

      explicit operator bool() const;

      using iterator = std::vector<Json>::iterator;
      using const_iterator = std::vector<Json>::const_iterator;
      using value_type = Json;
      iterator
      begin();
      iterator
      end();
      const_iterator
      begin() const;
      const_iterator
      end() const;
    };

    using Array = std::vector<Json>;
    using Object = std::unordered_map<std::string, Json>;
    using OrderedObject = std::map<std::string, Json>;

    class NullType
    {};

    template <typename Cont>
    auto
    make_array(const Cont& c)
      -> Array
    {
      using std::begin;
      using std::end;
      return {begin(c), end(c)};
    }

    /// Map `fun` on the elements of `c`, and return as a Json array.
    template <typename Cont, typename Fun>
    auto
    make_array(const Cont& c, Fun&& fun)
      -> Array
    {
      auto res = Array{};
      res.reserve(c.size());
      std::transform(c.begin(), c.end(),
                     std::back_inserter(res),
                     std::forward<Fun>(fun));
      return res;
    }

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

    std::ostream&
    operator <<(std::ostream& stream, Json const& json);

    std::ostream&
    operator <<(std::ostream& stream, elle::json::Object const& obj);

    std::ostream&
    operator <<(std::ostream& stream, elle::json::OrderedObject const& obj);
  }
}

#include <elle/json/json.hxx>
