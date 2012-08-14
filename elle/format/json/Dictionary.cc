#include <ostream>

#include "elle/serialize/JSONArchive.hxx"

#include "Dictionary.hh"

using namespace elle::format::json;

Dictionary::Dictionary(internal::Dictionary&& value):
  _map{value}
{}

void Dictionary::Save(elle::serialize::OutputJSONArchive& ar) const
{
  auto it = _map.begin(),
       end = _map.end();
  ar.stream() << '{';
  if (it != end)
    {
      ar << it->first;
      ar.stream() << ':';
      assert(it->second != nullptr);
      it->second->Save(ar);
      ++it;
      for (; it != end; ++it)
        {
          ar.stream() << ',';
          ar << it->first;
          ar.stream() << ':';
          it->second->Save(ar);
        }
    }
  ar.stream() << '}';
}

std::unique_ptr<Object> Dictionary::Clone() const
{
  auto res = std::unique_ptr<Dictionary>(new Dictionary);
  for (auto it = _map.begin(), end = _map.end(); it != end; ++it)
    {
      assert(it->second != nullptr);
      res->_map[it->first] = it->second->Clone().release();
    }
  return std::unique_ptr<Object>(res.release());
}

void Dictionary::update(Dictionary const& other)
{
  for (auto const& pair: other._map)
    {
      this->_map[pair.first] = pair.second->Clone().release();
    }
}
