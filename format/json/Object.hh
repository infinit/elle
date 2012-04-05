#ifndef ELLE_FORMAT_JSON_OBJECT_HH
# define ELLE_FORMAT_JSON_OBJECT_HH

# include <cstdint>
# include <memory>
# include <string>

# include "elle/serialize/JSONArchive.fwd.hh"

namespace elle { namespace format { namespace json {

    class Dictionary;
    class Array;

    /// Root object for all json types
    class Object
    {
    public:
      typedef elle::serialize::OutputJSONArchive OutputJSONArchive;
      typedef elle::serialize::InputJSONArchive  InputJSONArchive;

      // The json archive can save object
      friend class elle::serialize::OutputJSONArchive;
      friend class elle::serialize::InputJSONArchive;

      // Array and Dictionary class may save children objects
      friend class Dictionary;
      friend class Array;

    public:
      virtual ~Object() {}

    protected:
      virtual void Save(OutputJSONArchive& ar) const = 0;

    public:
      /// Get the value of an object value, when the object value
      /// can be casted without precision loss into the type T.
      /// For example, float to integer conversion WON'T work.
      ///
      /// You'll have to include "Object.hxx" in order to use this
      /// method.
      ///
      /// @throws std::bad_cast
      /// @see Object::TryLoad() for an exception safe version
      template<typename T> void Load(T& out) const;

      /// Same as Load(), but instead of throwing an exception,
      /// it returns false when conversion cannot be done.
      template<typename T> bool TryLoad(T& out) const;

      /// Cloning a JSON Object
      virtual std::unique_ptr<Object> Clone() const = 0;

      /// Returns the JSON representation of this object
      std::string repr() const;
    };

}}}

#endif /* ! OBJECT_HH */


