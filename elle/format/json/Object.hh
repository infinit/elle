#ifndef  ELLE_FORMAT_JSON_OBJECT_HH
# define ELLE_FORMAT_JSON_OBJECT_HH

# include <cstdint>
# include <memory>
# include <string>

# include <elle/serialize/fwd.hh>

# include "fwd.hh"

namespace elle
{
  namespace format
  {
    namespace json
    {

<<<<<<< HEAD
      /// Root object for all json types
      class Object
      {
      public:
        template<typename T> struct CanLoad;

        typedef elle::serialize::OutputJSONArchive OutputJSONArchive;
        typedef elle::serialize::InputJSONArchive  InputJSONArchive;

        // The json archive can save objects.
        friend class elle::serialize::OutputJSONArchive;
        friend class elle::serialize::InputJSONArchive;

        // Array and Dictionary class may save children objects.
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
        template <typename T> void Load(T& out) const;

        /// Convert an object to the type T and return by value.
        /// @throws std::bad_cast
        template <typename T> T as() const;

        inline Array&      as_array();
        inline Bool&       as_bool();
        inline Dictionary& as_dictionary();
        inline Float&      as_float();
        inline Integer&    as_integer();
        inline Null&       as_null();
        inline String&     as_string();

        inline Array const&      as_array() const;
        inline Bool const&       as_bool() const;
        inline Dictionary const& as_dictionary() const;
        inline Float const&      as_float() const;
        inline Integer const&    as_integer() const;
        inline Null const&       as_null() const;
        inline String const&     as_string() const;

        /// Same as Load(), but instead of throwing an exception,
        /// it returns false when conversion cannot be done.
        template<typename T> bool TryLoad(T& out) const;

        /// Cloning a JSON Object
        virtual std::unique_ptr<Object> Clone() const = 0;

        /// Returns the JSON representation of this object
        std::string repr() const;


      public:
        /// operator ==
        virtual bool operator ==(Object const& other) const = 0;
        virtual bool operator ==(Array const&) const      { return false; }
        virtual bool operator ==(Bool const&) const       { return false; }
        virtual bool operator ==(Dictionary const&) const { return false; }
        virtual bool operator ==(Float const&) const      { return false; }
        virtual bool operator ==(Integer const&) const    { return false; }
        virtual bool operator ==(Null const&) const       { return false; }
        virtual bool operator ==(String const&) const     { return false; }
        template <typename T>
        typename std::enable_if<!std::is_base_of<T, Object>::value, bool>::type
        operator ==(T const& other) const;

        /// operator !=
        bool operator !=(Object const& other) const       { return !(*this == other); }
        bool operator !=(Array const& other) const        { return !(*this == other); }
        bool operator !=(Bool const& other) const         { return !(*this == other); }
        bool operator !=(Dictionary const& other) const   { return !(*this == other); }
        bool operator !=(Float const& other) const        { return !(*this == other); }
        bool operator !=(Integer const& other) const      { return !(*this == other); }
        bool operator !=(Null const& other) const         { return !(*this == other); }
        bool operator !=(String const& other) const       { return !(*this == other); }
        template <typename T>
        bool operator !=(T const& other) const            { return !(*this == other); }

        /// Usable as a constant dictionary
        virtual Object const& operator [](std::string const& key) const;

        /// Usable as a constant array
        virtual Object const& operator [](size_t index) const;
      };
=======
    /// Root object for all json types
    class Object
    {
    public:
      template<typename T> struct CanLoad;

      typedef elle::serialize::OutputJSONArchive OutputJSONArchive;
      typedef elle::serialize::InputJSONArchive  InputJSONArchive;

      // The json archive can save objects.
      friend class elle::serialize::OutputJSONArchive;
      friend class elle::serialize::InputJSONArchive;

      // Array and Dictionary class may save children objects.
      friend class Dictionary;
      friend class Array;

      /// Used to build a JSON type from a type.
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
      template <typename T> void Load(T& out) const;

      /// Convert an object to the type T and return by value.
      /// @throws std::bad_cast
      template <typename T> T as() const;

      inline Array&      as_array();
      inline Bool&       as_bool();
      inline Dictionary& as_dictionary();
      inline Float&      as_float();
      inline Integer&    as_integer();
      inline Null&       as_null();
      inline String&     as_string();

      inline Array const&      as_array() const;
      inline Bool const&       as_bool() const;
      inline Dictionary const& as_dictionary() const;
      inline Float const&      as_float() const;
      inline Integer const&    as_integer() const;
      inline Null const&       as_null() const;
      inline String const&     as_string() const;

      /// Same as Load(), but instead of throwing an exception,
      /// it returns false when conversion cannot be done.
      template<typename T> bool TryLoad(T& out) const;

      /// Cloning a JSON Object
      virtual std::unique_ptr<Object> Clone() const = 0;

      /// Returns the JSON representation of this object
      std::string repr() const;


    public:
      /// operator ==
      virtual bool operator ==(Object const& other) const = 0;
      virtual bool operator ==(Array const&) const      { return false; }
      virtual bool operator ==(Bool const&) const       { return false; }
      virtual bool operator ==(Dictionary const&) const { return false; }
      virtual bool operator ==(Float const&) const      { return false; }
      virtual bool operator ==(Integer const&) const    { return false; }
      virtual bool operator ==(Null const&) const       { return false; }
      virtual bool operator ==(String const&) const     { return false; }
      template <typename T>
      typename std::enable_if<!std::is_base_of<T, Object>::value, bool>::type
      operator ==(T const& other) const;

      /// operator !=
      bool operator !=(Object const& other) const       { return !(*this == other); }
      bool operator !=(Array const& other) const        { return !(*this == other); }
      bool operator !=(Bool const& other) const         { return !(*this == other); }
      bool operator !=(Dictionary const& other) const   { return !(*this == other); }
      bool operator !=(Float const& other) const        { return !(*this == other); }
      bool operator !=(Integer const& other) const      { return !(*this == other); }
      bool operator !=(Null const& other) const         { return !(*this == other); }
      bool operator !=(String const& other) const       { return !(*this == other); }
      template <typename T>
      bool operator !=(T const& other) const            { return !(*this == other); }

      /// Usable as a constant dictionary
      virtual Object const& operator [](std::string const& key) const;

      /// Usable as a constant array
      virtual Object const& operator [](size_t index) const;
    };
>>>>>>> 5ac8980... start cleaning json format

    }
  }
}

# include "Object.hxx"

#endif /* ! OBJECT_HH */
