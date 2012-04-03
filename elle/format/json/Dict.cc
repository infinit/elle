#include <ostream>

#include "elle/serialize/JSONArchive.hh"

#include "Dict.hh"

using namespace elle::format::json;

void Dict::Save(elle::serialize::OutputJSONArchive& ar) const
{
  auto it = _map.begin(),
       end = _map.end();
  ar.stream() << '{';
  if (it != end)
    {
      ar << it->first;
      ar.stream() << ':';
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
std::unique_ptr<Object> Dict::Clone() const
{
  auto res = std::unique_ptr<Dict>(new Dict);
  for (auto it = _map.begin(), end = _map.end(); it != end; ++it)
    {
      res->_map[it->first] = it->second->Clone().release();
    }
  return std::unique_ptr<Object>(res.release());
}
