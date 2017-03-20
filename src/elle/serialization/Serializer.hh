#ifndef ELLE_SERIALIZATION_SERIALIZER_HH
# define ELLE_SERIALIZATION_SERIALIZER_HH

# include <functional>
# include <list>
# include <memory>
# include <set>
# include <string>
# include <type_traits>
# include <typeinfo>
# include <unordered_map>
# include <unordered_set>

# include <boost/any.hpp>
# include <boost/container/flat_set.hpp>
# include <boost/date_time/posix_time/posix_time.hpp>
# include <boost/multi_index_container.hpp>

# include <elle/Buffer.hh>
# include <elle/TypeInfo.hh>
# include <elle/Version.hh>
# include <elle/attribute.hh>
# include <elle/err.hh>
# include <elle/log.hh>
# include <elle/optional.hh>
# include <elle/serialization/fwd.hh>
# include <elle/sfinae.hh>

namespace elle
{
  namespace serialization
  {
    template <typename T>
    class as;

    template <typename T>
    struct Serialize
    {};

    /// The context of a Serializer, to "inject" context when deserializing an
    /// object.
    ///
    /// Objects you want to serialize are not always PODs. Some can contain
    /// references or pointers to other objects. Context can "inject" values
    /// when deserializing the object, to recreate the object "as" is was
    /// serialized.
    ///
    /// N.B. You can have only one instance of a given type per Context.
    ///
    /// \code{.cc}
    ///
    /// struct Foo
    /// {
    ///   Foo(elle::serialization::SerializerIn& input)
    ///   {
    ///     this->serialize(input);
    ///   }
    ///
    ///   void
    ///   serialize(elle::serialization::Serializer& s)
    ///   {
    ///     s.serialize_context<Master const*>(this->_master);
    ///     s.serialize("msg", this->_msg);
    ///   }
    /// private:
    ///   ELLE_ATTRIBUTE_R(std::string, msg);
    ///   ELLE_ATTRIBUTE_R(Master const*, master);
    /// };
    ///
    /// /...
    ///
    /// std::stringstream stream(
    ///   "{"
    ///   "  \"msg\": \"/r/nocontext\""
    ///   "}");
    ///
    /// json::SerializerIn serializer(stream, false);
    /// serializer.template set_context<Master const*>(&m);
    /// Context ctx(serializer);
    /// BOOST_CHECK_EQUAL(ctx.msg(), "/r/nocontext");
    /// assert(ctx.master()->operational());
    ///
    /// \endcode
    class ELLE_API Context
    {
    public:
      /// Create a context.
      template <typename... Args>
      Context(Args&&... args)
      {
        using swallow = int[];
        (void) swallow
        {
          (this->set(std::forward<Args>(args)), 0)...
        };
      }

      /// Register @a value to the context.
      ///
      /// @param value The value to register.
      template <typename T>
      void
      set(T value)
      {
        this->_value[type_info<T>()] = std::move(value);
      }

      /// Get @a value from the context.
      ///
      /// @param value A reference to a value to be filled.
      template <typename T>
      void
      get(T& value)
      {
        auto ti = type_info<T>();
        auto it = this->_value.find(ti);
        if (it == this->_value.end())
          elle::err("missing serialization context for %s", ti.name());
        value = boost::any_cast<T>(it->second);
      }

      /// Get @a value from the Context. If it's not in the Context, use @a
      /// default value given.
      ///
      /// @param value A reference to a value to be filled.
      /// @param default_value The default value.
      template <typename T>
      void
      get(T& value, const T& default_value)
      {
        auto it = this->_value.find(type_info<T>());
        if (it == this->_value.end())
          value = default_value;
        else
          value = boost::any_cast<T>(it->second);
      }

      /// Check whether a Context has an entry of type T.
      ///
      /// @tparam T The type of the entry you are checking.
      /// @returns Whether an entry exists.
      template <typename T>
      bool
      has() const
      {
        return this->_value.find(type_info<T>()) != this->_value.end();
      }

      /// Add @a given Context (adds the values it knows about).
      ///
      /// @param source A context.
      /// @returns This Context, with the extra entries from the other Context.
      Context&
      operator += (Context const& source)
      {
        for (auto const& e: source._value)
          this->_value.insert(e);
        return *this;
      }

      /// Get the number of entries in the Context.
      size_t
      size() const
      {
        return this->_value.size();
      }

      ELLE_ATTRIBUTE((std::map<TypeInfo, boost::any>), value);
    };

    /// An abstract Serializer in charge of both serializing and deserializing
    /// data.
    ///
    /// The Serializer contains methods to serialize (and deserialize) common
    /// types. A conversion API exists to enhance Serializer supported types.
    ///
    /// The Serializer entry point for serializing or deserializing a class is
    /// the method serialize of the class and a constructor taking a
    /// SerializerIn& as argument.
    ///
    /// The Serializer also support inheritance,
    /// @see elle::serialization::Hierarchy
    ///
    /// The Serializer support versioning. Classes with a serialization_tag
    /// (@see elle::serialization_tag) will be serialized at version
    /// serialization_tag::version(). When deserializing, one can access the
    /// Version by using the "serialize" method
    /// serialize(Serializer& s, elle::Version v);
    ///
    /// \code{.cc}
    ///
    /// struct NonVersionedFoo
    /// {
    ///   NonVersionedFoo(std::string const& bob, std::string const& alice)
    ///     : bob(bob)
    ///     , alice(alice)
    ///   {}
    ///
    ///   NonVersionedFoo(elle::serialization::SerializerIn& s)
    ///   {
    ///     this->serialize(s);
    ///   }
    ///
    ///   virtual
    ///   ~NonVersionedFoo() = default;
    ///
    ///   void
    ///   serialize(elle::serialization::Serializer& s)
    ///   {
    ///     std::cout << (s.in() ? "de" : "") << "serializing "
    ///               << elle::type_info(*this) << "\n";
    ///     s.serialize("bob", this->bob);
    ///     s.serialize("alice", this->alice);
    ///   }
    ///
    ///   std::string bob;
    ///   std::string alice;
    /// };
    ///
    /// struct InheritedFoo
    ///   : public NonVersionedFoo
    /// {
    ///   InheritedFoo(elle::serialization::SerializerIn& s)
    ///     // DO NOT CALL THE PARENT CONSTRUCTOR THAT TAKES A SERIALIZER.
    ///     : NonVersionedFoo("", "")
    ///   {
    ///     this->serialize(s);
    ///   }
    ///
    ///   InheritedFoo(std::string const& eve)
    ///     : NonVersionedFoo("Inherited bob", "Inherited alice")
    ///     , eve(eve)
    ///   {}
    ///
    ///   void
    ///   serialize(elle::serialization::Serializer& s)
    ///   {
    ///     NonVersionedFoo::serialize(s);
    ///     s.serialize("eve", this->eve);
    ///   }
    ///
    ///   std::string eve;
    /// };
    ///
    /// static const elle::serialization::Hierarchy<NonVersionedFoo>::
    ///   Register<InheritedFoo> _register_inherited_foo;
    ///
    /// // The serialization used by the project.
    /// struct serialization_tag
    /// {
    ///   static const elle::Version version;
    /// };
    /// const elle::Version serialization_tag::version = {0, 4, 0};
    ///
    /// struct VersionedFoo
    /// {
    ///   VersionedFoo(std::string const& bob,
    ///                std::string const& alice,
    ///                std::string const& eve)
    ///     : bob(bob)
    ///     , alice(alice)
    ///     , eve(eve)
    ///   {}
    ///
    ///   VersionedFoo(elle::serialization::SerializerIn& s,
    ///               elle::Version const& v)
    ///   {
    ///     this->serialize(s, v);
    ///   }
    ///
    ///   void
    ///   serialize(elle::serialization::Serializer& s,
    ///             elle::Version const& v)
    ///   {
    ///     std::cout << (s.in() ? "de" : "") << "serializing "
    ///               << elle::type_info(*this) << "\n";
    ///     s.serialize("bob", this->bob);
    ///     s.serialize("alice", this->alice);
    ///     if (v >= elle::Version{0, 3, 0})
    ///       s.serialize("eve", this->eve);
    ///   }
    ///
    ///   std::string bob;
    ///   std::string alice;
    ///   // Attribute added at version 0.3.0.
    ///   boost::operational<std::string> eve;
    ///
    ///   // Register the serialization_tag.
    ///   using serialization_tag = ::serialization_tag;
    /// };
    ///
    /// // ...
    ///
    /// {
    ///   NonVersionedFoo f("bob", "alice");
    ///   std::stringstream o;
    ///   elle::serialization::json::serialize(f, o, false);
    ///   std::cout << o.str() << '\n';
    /// }
    /// {
    ///   InheritedFoo f("eve");
    ///   std::stringstream o;
    ///   elle::serialization::json::serialize(f, o, false);
    ///   std::cout << o.str() << '\n';
    /// }
    /// {
    ///   VersionedFoo f("bob", "alice", "eve");
    ///   {
    ///     std::stringstream o;
    ///     elle::serialization::json::serialize(f, o, true);
    ///     std::cout << o.str() << '\n';
    ///   }
    ///   {
    ///     std::stringstream o;
    ///     elle::serialization::json::serialize(f, o, elle::Version{0, 2, 0}, true);
    ///     std::cout << o.str() << '\n';
    ///   }
    /// }
    ///
    /// // Result:
    /// // serializing NonVersionedFoo
    /// {"alice":"alice","bob":"bob"}
    ///
    /// // serializing InheritedFoo
    /// {"alice":"Inherited alice","bob":"Inherited bob","eve":"eve"}
    ///
    /// // serializing VersionedFoo at version 0.4.0
    /// {".version":"0.4.0","alice":"alice","bob":"bob","eve":"eve"}
    ///
    /// // serializing VersionedFoo at version 0.2.0
    /// {".version":"0.2.0","alice":"alice","bob":"bob"}
    ///
    /// \endcode
    class ELLE_API Serializer
    {
    /*------.
    | Types |
    `------*/
    public:
      using Self = Serializer;
      using Versions = std::unordered_map<TypeInfo, Version>;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      /// Create a Serializer.
      ///
      /// @param versioned Whether the Serializer will read or write the
      ///                  version of objects.
      Serializer(bool versioned);
      /// Create a Serializer.
      ///
      /// @param versions A map of special Versions for given types.
      /// @param versioned Whether the Serializer will read or write the
      ///                  version of objects.
      Serializer(Versions versions, bool versioned);

    /*-----------.
    | Properties |
    `-----------*/
    public:
      /// Whether is a SerializerIn.
      bool
      in() const;
      /// Whether is a SerializerOut.
      virtual
      bool
      out() const = 0;
      /// Whether the serialization format is text (like JSON, unlike binary).
      ///
      /// Call _text.
      bool
      text() const;
      ELLE_ATTRIBUTE_R(bool, versioned);
      ELLE_ATTRIBUTE_R(boost::optional<Versions>, versions);

    /*--------------.
    | Serialization |
    `--------------*/
    public:
      /// Serialize or deserialize an arbitrary type T.
      ///
      /// @tparam Serializer The type of Serializer.
      /// @tparam T The type of the data to (de)serialize.
      /// @param name The name of the entry.
      /// @param v The variable to serialize or to deserialize to.
      template <typename Serializer = void, typename T>
      void
      serialize(std::string const& name, T& v);
      /// Serialize or deserialize an arbitrary boost::optional.
      ///
      /// @tparam Serializer The type of Serializer.
      /// @tparam T The type of the boost::optional underlying data to
      ///           (de)serialize.
      /// @param name The name of the entry.
      /// @param v The variable to serialize or to deserialize to.
      template <typename Serializer = void, typename T>
      void
      serialize(std::string const& name, boost::optional<T>& opt);
      /// Serialize or deserialize an arbitrary std::unique_ptr.
      ///
      /// @tparam Serializer The type of Serializer.
      /// @tparam T The type of the object managed by the std::unique_ptr.
      /// @tparam D The type of the deleter used by the std::unique_ptr.
      /// @param name The name of the entry.
      /// @param v The variable to serialize or to deserialize to.
      template <typename Serializer = void, typename T, typename D>
      void
      serialize(std::string const& name, std::unique_ptr<T, D>& opt);
      /// Serialize or deserialize an arbitrary std::shared_ptr.
      ///
      /// @tparam Serializer The type of Serializer.
      /// @tparam T The type of the object managed by the std::shared_ptr.
      /// @param name The name of the entry.
      /// @param v The variable to serialize or to deserialize to.
      template <typename Serializer = void, typename T>
      void
      serialize(std::string const& name, std::shared_ptr<T>& opt);
      /// Serialize or deserialize a naked pointer.
      ///
      /// @tparam Serializer The type of Serializer.
      /// @tparam T The type of the pointer.
      /// @param name The name of the entry.
      /// @param v The variable to serialize or to deserialize to.
      template <typename Serializer = void, typename T>
      std::enable_if_t<
        !_details::has_serialize_convert_api<T*, void>(), void>
      serialize(std::string const& name, T*& opt);
      /// Serialize or deserialize a object as a given type As.
      ///
      /// @tparam Serializer The type of Serializer.
      /// @tparam T The original type of the object.
      /// @tparam As The type to (de)serialize the object as.
      /// @param name The name of the entry.
      /// @param v The collection to serialize or to deserialize to.
      /// @param _ An instance of `as`.
      template <typename Serializer = void, typename T, typename As>
      void
      serialize(std::string const& name, T& v, as<As>);
      /// Serialize or deserialize a collection as a collection of given type
      /// As.
      ///
      /// @tparam As The type to (de)serialize elements as.
      /// @tparam C The type of the collection.
      /// @tparam T The original type of elements.
      /// @tparam A The type of the allocator for the elements.
      /// @tparam As The type to (de)serialize elements as.
      /// @param name The name of the entry.
      /// @param v The collection to serialize or to deserialize to.
      /// @param _ An instance of `as`.
      template <typename As,
                template <typename, typename> class C,
                typename T,
                typename A>
      std::enable_if_t<std::is_default_constructible<T>::value, void>
      serialize(std::string const& name, C<T, A>& collection, as<As>);
      /// Serialize or deserialize an object.
      ///
      /// XXX[doc]: Should this be in the public API?
      ///
      /// @tparam T The type of the object to (de)serialize.
      /// @param v The object to serialize or to deserialize to.
      template <typename T>
      void
      serialize_object(T& v);
      /// Serialize or deserialize a version.
      ///
      /// This is a special case of serialize<T> because we don't version
      /// versions.
      ///
      /// @param v The version to serialize or to deserialize to.
      void
      serialize_object(elle::Version& v);
      /// Serialize or deserialize a variant (such as elle::Option).
      ///
      /// elle::Option is represented by its value and the different types if
      /// can takes.
      ///
      /// @param names The names of the types the variant can take.
      /// @param index The index of the actual type (out: the index, in: -1).
      /// @param f The function to serialize the variant.
      void
      serialize_variant(std::vector<std::string> const& names,
                        int index, // out: filled, in: -1
                        std::function<void(int)> const& f);
      ///
      template <typename T>
      void
      serialize_ptr(std::string const& name, T* &v, bool anonymous = false);
      template<typename T>
      void
      serialize_with_struct_serialize(std::string const& name,
                                      T& v,
                                      bool anonymous);
      /// Serialize or deserialize an unnamed value.
      ///
      /// @tparam Serializer The type of Serializer.
      /// @tparam T The original type of the object.
      /// @param v The variable to serialize or to deserialize to.
      template <typename Serializer = void, typename T>
      void
      serialize_forward(T& v);
      /// Get the value of type T from the Context.
      ///
      /// N.B. If the serializer is in input mode, this method does nothing.
      ///
      /// @param T The type of the value to get from the context.
      /// @param value The variable where to store the value from the Context.
      template <typename T>
      void
      serialize_context(T& value);
      /// Set a value for a given type T in the Context.
      ///
      /// @tparam T The type of the value to store.
      /// @param value The value to store.
      template <typename T>
      void
      set_context(T value);
      /// Add @a given Context to our Context.
      ///
      /// @param context The Context to add.
      void
      set_context(Context const& context);
      ELLE_ATTRIBUTE_R(Context, context);
      /// A dispatcher method.
      ///
      /// XXX[doc]: Should this be in the public API?
      ///
      /// @tparam S
      /// @tparam Serializer The type of Serializer.
      /// @tparam T The type of the object.
      /// @param s The serializer to use.
      /// @param v The variable to serialize or to deserialize to.
      template <typename S = void, typename Serializer, typename T>
      static
      void
      serialize_switch(Serializer& s, T& v);

    /*------------.
    | Enter/leave |
    `------------*/
    public:
      /// Represent an entry in the serialization, to smooth hierarchy
      /// exploration.
      ///
      /// enter signals the serializer we are (de)serializing an entry.
      /// leave signals we are done with the entry.
      class Entry
      {
      public:
        ///
        ~Entry();
        /// Always return true, to support the pattern.
        /// if (auto entry = Entry(s, name))
        /// ...
        operator bool() const;
      private:
        /// Construct an Entry for @a Serializer for @a given name.
        ///
        /// @param s The Serializer.
        /// @param name The name of the Entry.
        Entry(Serializer& s, std::string const& name);
        friend class Serializer;
        ELLE_ATTRIBUTE(Serializer&, serializer);
        ELLE_ATTRIBUTE(elle::log::detail::Send, log);
        ELLE_ATTRIBUTE(std::string const&, name);
        ELLE_ATTRIBUTE(bool, entered);
      };
      /// Create an Entry (for this Serializer).
      ///
      /// @param name The name of the Entry.
      /// @returns An Entry.
      Entry
      enter(std::string const& name);
      /// Get the name of the current Entry.
      std::string
      current_name() const;
    protected:
      /// Call when entering an entry or a collection.
      ///
      /// XXX[doc].
      ///
      /// @param name The name of the entry.
      /// @return Whether entering was a success or not.
      virtual
      bool
      _enter(std::string const& name);
      /// Call when leaving an entry of a collection.
      ///
      /// XXX[doc].
      ///
      /// @param name The name of the entry we are leaving.
      virtual
      void
      _leave(std::string const& name);
      ELLE_ATTRIBUTE(std::vector<std::string>, names, protected);

    protected:
      /// XXX[doc].
      virtual
      void
      _size(int size);
      /// Whether the serialization protocol is textual (like JSON).
      virtual
      bool
      _text() const;
      /// Call when serializing or deserializing an array.
      ///
      /// @param size The size of the array to serialize, -1 if we are
      ///             deserializing (this->in() == true).
      /// @param f The function to serialize elements.
      virtual
      void
      _serialize_array(int size,
                       std::function<void ()> const& f) = 0;
      /// Call when serializing an entry of dictionary.
      ///
      /// @param name The name of the entry.
      /// @param f The function serializing the element.
      virtual
      void
      _serialize_dict_key(std::string const& name,
                          std::function<void ()> const& f);
      /// Call when deserializing an entry of dictionary.
      ///
      /// @param f The function to deserialize an element.
      virtual
      void
      _deserialize_dict_key(
      std::function<void (std::string const&)> const& f);
      /// Serialize or deserialize an int64_t;
      virtual
      void
      _serialize(int64_t& v) = 0;
      /// Serialize or deserialize an uint64_t;
      virtual
      void
      _serialize(uint64_t& v) = 0;
      /// Serialize or deserialize an int32_t;
      virtual
      void
      _serialize(int32_t& v) = 0;
      /// Serialize or deserialize an uint32_t;
      virtual
      void
      _serialize(uint32_t& v) = 0;
      /// Serialize or deserialize an int16_t;
      virtual
      void
      _serialize(int16_t& v) = 0;
      /// Serialize or deserialize an uint16_t;
      virtual
      void
      _serialize(uint16_t& v) = 0;
      /// Serialize or deserialize an int8_t;
      virtual
      void
      _serialize(int8_t& v) = 0;
      /// Serialize or deserialize an uint8_t;
      virtual
      void
      _serialize(uint8_t& v) = 0;
      /// Serialize or deserialize a double.
      virtual
      void
      _serialize(double& v) = 0;
      /// Serialize or deserialize a bool.
      virtual
      void
      _serialize(bool& v) = 0;
      /// Serialize or deserialize a std::string.
      virtual
      void
      _serialize(std::string& v) = 0;
      /// Serialize or deserialize a elle::Buffer.
      virtual
      void
      _serialize(elle::Buffer& v) = 0;
      /// Serialize or deserialize a elle::WeakBuffer.
      void
      _serialize(elle::WeakBuffer& v);
      /// Serialize or deserialize a boost::posix_time::ptime.
      virtual
      void
      _serialize(boost::posix_time::ptime& v) = 0;
      /// Serialize or deserialize an elle::Duration.
      template <typename Repr, typename Ratio>
      void
      _serialize(std::chrono::duration<Repr, Ratio>& duration);
      /// Serialize or deserialize a Duration type from its ticks, numerator
      /// and denominator.
      ///
      /// @param ticks The number of ticks.
      /// @param ticks The numerator of the ratio (number of seconds per tick).
      /// @param ticks The denominator of the ratio (number of seconds per
      ///              tick).
      virtual
      void
      _serialize_time_duration(std::int64_t& ticks,
                               std::int64_t& num,
                               std::int64_t& denom) = 0;
      /// Serialize a named optional entry.
      ///
      /// @param name The name of the entry.
      /// @param present Whether the option is present of not.
      /// @param f The function to serialize the object.
      virtual
      void
      _serialize_named_option(std::string const& name,
                              bool present,
                              std::function<void ()> const& f) = 0;
      /// Serialize an anonymous optional entry.
      ///
      /// @param present Whether the option is present of not.
      /// @param f The function to serialize the object.
      virtual
      void
      _serialize_option(bool present,
                        std::function<void ()> const& f) = 0;
      /// Serialize or deserialize a variant (such as elle::Option).
      ///
      /// @param names The names of the types the variant can take.
      /// @param index The index of the actual type (out: the index, in: -1).
      /// @param f The function to serialize the variant.
      virtual
      void
      _serialize_variant(std::vector<std::string> const& names,
                         int index, // out: filled, in: -1
                         std::function<void(int)> const& f);
      /// Serialize or deserialize an arbitrary collection.
      ///
      /// @tparam S XXX[doc]
      /// @tparam C The type of the collection.
      /// @tparam T The type of the elements the collection stores.
      /// @tparam A The type of the allocator for elements.
      /// @param collection The collection.
      template <typename S = void,
                template <typename, typename> class C,
                typename T, typename A>
      void
      _serialize(C<T, A>& collection);
      /// Serialize or deserialize an arbitrary collection.
      ///
      /// @tparam S XXX[doc]
      /// @tparam C The type of the collection.
      /// @param collection The collection.
      template <typename S = void, typename C>
      void
      _serialize_collection(C& collection);
      /// Serialize or deserialize a vector.
      ///
      /// @tparam S XXX[doc]
      /// @tparam T The type of the elements the vector stores.
      /// @tparam A The type of the allocator for the elements.
      /// @param collection A vector.
      template <typename S = void, typename T, typename A>
      void
      _serialize(std::vector<T, A>& collection);
      /// Serialize or deserialize a set.
      ///
      /// @tparam T The type of the elements the set stores.
      /// @tparam C The type of the comparison function.
      /// @tparam A The type of the allocator for the elements.
      /// @param collection A set.
      template <typename T, typename C, typename A>
      void
      _serialize(std::set<T, C, A>& collection);
      /// Serialize or deserialize a boost::container::flat_set.
      ///
      /// @tparam Args... The flat_set template parameters.
      /// @param collection A flat set.
      template <typename... Args>
      void
      _serialize(boost::container::flat_set<Args...>& collection);
      /// Serialize or deserialize a boost::multi_index_container.
      ///
      /// @tparam T The type of the elements the bmi stores.
      /// @tparam I The index specifier list.
      /// @param collection A bmi.
      template <typename T, typename I>
      void
      _serialize(boost::multi_index::multi_index_container<T, I>& collection);
      /// Serialize or deserialize an arbitrary collection as the given type As.
      ///
      /// @tparam As The type to (de)serialize the object as.
      /// @tparam C The type of the collection.
      /// @tparam T The type of elements the collection stores.
      /// @tparam A The type of the allocator for elements.
      /// @param collection The collection.
      /// @param _ An instance of `as`.
      template <typename As,
                template <typename, typename> class C, typename T, typename A>
      std::enable_if_t<std::is_default_constructible<T>::value, void>
      _serialize(C<T, A>& collection, as<As>);
      /// Serialize or deserialize a std::pair.
      ///
      /// @tparam T1 The type of the first element the the pair stores.
      /// @tparam T2 the type of the second element the the pair stores.
      /// @param collection The pair.
      template <typename T1, typename T2>
      void
      _serialize(std::pair<T1, T2>& collection);
      /// Serialize or deserialize a std::unordered_map.
      ///
      /// @tparam K The type of the key.
      /// @tparam V the type of the value the the unordered_map stores.
      /// @tparam ...Rest The rest of the template arguments.
      /// @param map The unordered_map.
      template <typename K, typename V, typename ... Rest>
      void
      _serialize(std::unordered_map<K, V, Rest...>& map);
      /// Serialize or deserialize a std::unordered_map whose Key is a string.
      ///
      /// @tparam V the type of the value the the unordered_map stores.
      /// @tparam ...Rest The rest of the template arguments.
      /// @param map The unordered_map.
      template <typename V, typename ... Rest>
      void
      _serialize(std::unordered_map<std::string, V, Rest...>& map);
      /// Serialize or deserialize a std::map.
      ///
      /// @tparam K The type of the key.
      /// @tparam V the type of the value the the map stores.
      /// @tparam ...Rest The rest of the template arguments.
      /// @param map The map.
      template <typename K, typename V, typename ... Rest>
      void
      _serialize(std::map<K, V, Rest...>& map);
      /// Serialize or deserialize a std::map whose Key is a string.
      ///
      /// @tparam V the type of the value the the map stores.
      /// @tparam ...Rest The rest of the template arguments.
      /// @param map The map.
      template <typename V, typename ... Rest>
      void
      _serialize(std::map<std::string, V, Rest...>& map);
      /// Serialize or deserialize a unordered_set.
      ///
      /// @tparam T The type of the elements the unordered_set stores.
      /// @tparam A The type of the allocator for the elements.
      /// @param set The set.
      template <typename V, typename ... Rest>
      void
      _serialize(std::unordered_set<V, Rest...>& set);
      /// Serialize or deserialize a std::unordered_multimap.
      ///
      /// @tparam K The type of the key.
      /// @tparam V the type of the value the the unordered_multimap stores.
      /// @tparam ...Rest The rest of the template arguments.
      /// @param map The unordered_multimap.
      template <typename K, typename V, typename ... Rest>
      void
      _serialize(std::unordered_multimap<K, V, Rest...>& map);
      /// Serialize or deserialize an arbitrary associative container.
      ///
      /// @tparam C The type of the container.
      /// @param map The container.
      template <typename C>
      void
      _serialize_assoc(C& map);
      /// Serialize an arbitrary exception.
      ///
      /// @param e An exception_ptr.
      void
      _serialize_anonymous_exception(std::exception_ptr& e);
      /// Deserialize and emplace a value in a collection.
      ///
      /// @tparam S
      /// @tparam C The type of the collection.
      /// @param collection The collection.
      template <typename S = void, typename C>
      std::enable_if_exists_t<
        decltype(
          std::declval<C>().emplace(
            std::declval<elle::serialization::SerializerIn>())),
        void>
      _deserialize_in_array(C& collection);
      /// Deserialize and emplace_back a value in a collection.
      ///
      /// @tparam S
      /// @tparam C The type of the collection.
      /// @param collection The collection.
      template <typename S = void, typename C>
      std::enable_if_exists_t<
        decltype(
          std::declval<C>().emplace_back(
            std::declval<elle::serialization::SerializerIn>())),
        void>
      _deserialize_in_array(C& collection);
      template <typename T>
      friend struct Serialize;
      class Details;
      friend class Details;
    };

    template <typename T>
    class as
    {};

    template <typename T>
    class ELLE_API Hierarchy;
  }
}

# if !defined(ELLE_SERIALIZATION_SERIALIZER_IN_HH) && !defined(ELLE_SERIALIZATION_SERIALIZER_OUT_HH)
#  include <elle/serialization/Serializer.hxx>
# endif

#endif
