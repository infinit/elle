#include <horizon/macosx/Crib.hh>

#include <elle/standalone/Report.hh>

namespace horizon
{
  namespace macosx
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this container holds the set of created files along with their path
    /// and handle.
    ///
    Crib::Container                 Crib::Handles;

//
// ---------- static methods --------------------------------------------------
//

///
/// this method adds a created file.
///
    elle::Status            Crib::Add(const elle::String&           path,
                                      Handle*                       handle)
    {
      std::pair<Crib::Iterator, elle::Boolean>   result;

      result = Crib::Handles.insert(
        std::pair<const elle::String, Handle*>(path, handle));

      if (result.second == false)
        escape("unable to insert the scope in the container");

      return elle::Status::Ok;
    }

    ///
    /// this method returns true if the created file, referenced through
    /// the given path, exists.
    ///
    elle::Status            Crib::Exist(const elle::String&         path)
    {
      Crib::Iterator        iterator;

      if ((iterator = Crib::Handles.find(path)) != Crib::Handles.end())
        return elle::Status::True;

      return elle::Status::False;
    }

    ///
    /// this method returns the handle associated with the given path.
    ///
    elle::Status            Crib::Retrieve(const elle::String&      path,
                                           Handle*&                 handle)
    {
      Crib::Iterator        iterator;

      if ((iterator = Crib::Handles.find(path)) == Crib::Handles.end())
        escape("unable to locate the scope associated with the given chemin");

      handle = iterator->second;

      return elle::Status::Ok;
    }

    ///
    /// this method removes the created file's handle from the crib.
    ///
    elle::Status            Crib::Remove(const elle::String&        path)
    {
      Crib::Iterator        iterator;

      if ((iterator = Crib::Handles.find(path)) == Crib::Handles.end())
        escape("unable to locate the scope associated with the given chemin");

      Crib::Handles.erase(iterator);

      return elle::Status::Ok;
    }

  }
}
