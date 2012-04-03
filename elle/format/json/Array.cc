#include <ostream>

#include "elle/serialize/JSONArchive.hh"

#include "Array.hh"

using namespace elle::format::json;

void Array::Save(elle::serialize::OutputJSONArchive& ar) const
{
  ar.stream() << '[';
  auto it = _value.begin(), end = _value.end();
  if (it != end)
    {
      (*it)->Save(ar);
      ++it;
      for (; it != end; ++it)
        {
          ar.stream() << ',';
          (*it)->Save(ar);
        }
    }
  ar.stream() << ']';
}

std::unique_ptr<Object> Array::Clone() const
{
  auto res = std::unique_ptr<Array>(new Array);
  for (auto it = _value.begin(), end = _value.end(); it != end; ++it)
    {
      res->_value.push_back(
        (*it)->Clone().release()
      );
    }
  return std::unique_ptr<Object>(res.release());
}
