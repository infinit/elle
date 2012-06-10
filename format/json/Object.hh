#ifndef ELLE_FORMAT_JSON_OBJECT_HH
# define ELLE_FORMAT_JSON_OBJECT_HH

# include <cstdint>
# include <memory>
# include <string>

# include "elle/serialize/fwd.hh"

namespace elle { namespace format { namespace json {

    namespace detail {

        template<typename T> class BasicObject;

    }

    typedef detail::BasicObject<int32_t>        Integer;
    typedef detail::BasicObject<bool>           Bool;
    typedef detail::BasicObject<double>         Float;
    typedef detail::BasicObject<std::string>    String;

    class Array;
    class Dictionary;
    class Null;

    /// Root object for all json types
    class Object
    {
    public:
      template<typename T> struct CanLoad;

      typedef elle::serialize::OutputJSONArchive OutputJSONArchive;
      typedef elle::serialize::InputJSONArchive  InputJSONArchive;

      // The json archive can save object
      friend class elle::serialize::OutputJSONArchive;
      friend class elle::serialize::InputJSONArchive;

      // Array and Dictionary class may save children objects
      friend class Dictionary;
      friend class Array;

      /// Used to build a JSON type form a type
      struct Factory;

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


    public:
      /// double dispatch
      virtual bool operator ==(Object const& other) const = 0;
      virtual bool operator ==(Array const&) const      { return false; }
      virtual bool operator ==(Bool const&) const       { return false; }
      virtual bool operator ==(Dictionary const&) const { return false; }
      virtual bool operator ==(Float const&) const      { return false; }
      virtual bool operator ==(Integer const&) const    { return false; }
      virtual bool operator ==(Null const&) const       { return false; }
      virtual bool operator ==(String const&) const     { return false; }
      template<typename T> typename std::enable_if<
            !std::is_base_of<T, Object>::value
          , bool
      >::type operator ==(T const& other) const;

      bool operator !=(Object const& other) const       { return !(*this == other); }
      bool operator !=(Array const& other) const        { return !(*this == other); }
      bool operator !=(Bool const& other) const         { return !(*this == other); }
      bool operator !=(Dictionary const& other) const   { return !(*this == other); }
      bool operator !=(Float const& other) const        { return !(*this == other); }
      bool operator !=(Integer const& other) const      { return !(*this == other); }
      bool operator !=(Null const& other) const         { return !(*this == other); }
      bool operator !=(String const& other) const       { return !(*this == other); }
      template<typename T>
        bool operator !=(T const& other) const          { return !(*this == other); }

    };

}}}

#endif /* ! OBJECT_HH */


