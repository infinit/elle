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
    class NullType
    {};

    using Boolean = bool;
    using Integer = int64_t;
    using Real = double;
    using String = std::string;

    class Json
      : public boost::any
    {
    public:
      Json() = default;
      Json(Json&& j) = default;
      Json(Json& j) = default;
      Json(Json const& j) = default;
      Json(Boolean v);
      Json(int32_t v);
      Json(uint32_t v);
      Json(int64_t v);
      Json(uint64_t v);
      Json(long long int v);
      Json(long long unsigned int v);
      Json(NullType v);
      Json(Real v);
      Json(std::map<std::string, Json> v);
      Json(std::string v);
      Json(std::unordered_map<std::string, Json> v);
      Json(std::vector<Json> v);
      template <typename T>
      explicit
      Json(T&& v);

      Integer&
      integer();
      Integer const&
      integer() const;

      String&
      string();
      String const&
      string() const;

      std::vector<Json>&
      array();
      std::vector<Json> const&
      array() const;

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
