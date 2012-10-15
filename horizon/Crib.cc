#include <elle/log.hh>

#include <horizon/Crib.hh>

ELLE_LOG_COMPONENT("infinit.horizon.Crib");

namespace horizon
{
  /// The set of created files along with their path and handle.
  Crib::Container                 Crib::Handles;

  /// Add a created file.
  elle::Status
  Crib::Add(const elle::String& path,
            Handle* handle)
  {
    ELLE_TRACE_SCOPE("add handle for %s: %s", path, handle);

    std::pair<Crib::Iterator, elle::Boolean>   result;

    result = Crib::Handles.insert(
      std::pair<const elle::String, Handle*>(path, handle));

    if (result.second == false)
      escape("unable to insert the scope in the container");

    return elle::Status::Ok;
  }

  /// True if the created file referenced through the given path
  /// exists.
  elle::Status
  Crib::Exist(const elle::String& path)
  {
    ELLE_TRACE_SCOPE("check if handle exists for %s", path);

    Crib::Iterator iterator;

    if ((iterator = Crib::Handles.find(path)) != Crib::Handles.end())
      return elle::Status::True;

    return elle::Status::False;
  }

  /// The handle associated with the given path.
  elle::Status
  Crib::Retrieve(const elle::String& path,
                 Handle*& handle)
  {
    ELLE_TRACE_SCOPE("retrieve handle for %s", path);

    Crib::Iterator        iterator;

    if ((iterator = Crib::Handles.find(path)) == Crib::Handles.end())
      escape("unable to locate the scope associated with the given chemin");

    handle = iterator->second;

    return elle::Status::Ok;
  }

  /// Remove the created file's handle.
  elle::Status
  Crib::Remove(const elle::String& path)
  {
    ELLE_TRACE_SCOPE("remove handle for %s", path);

    Crib::Iterator        iterator;

    if ((iterator = Crib::Handles.find(path)) == Crib::Handles.end())
      escape("unable to locate the scope associated with the given chemin");

    Crib::Handles.erase(iterator);

    return elle::Status::Ok;
  }

  void
  Crib::rename(const char* source,
               const char* target)
  {
    auto elt = Handles.find(source);
    assert(elt != Handles.end());
    auto res = Handles.insert(Container::value_type(target, elt->second));
    assert(res.second);
    Handles.erase(elt);
  }
}
