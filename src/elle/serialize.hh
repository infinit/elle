#ifndef  ELLE_SERIALIZE_HH
# define ELLE_SERIALIZE_HH

/**
 * @namespace   elle::serialize
 * @brief       Serialization module
 *
 * elle.serialize is a library that aims to provide a fast, portable and safe
 * way to convert object from a format to another.
 *
 * Vocabulary and concepts
 * =======================
 *
 *   We use the term *archive* do designate the object that convert a object to
 *   its *representation* in another *format*. There are in fact one archive
 *   type for each provided format. For example, the base64 format if handled
 *   by a specialized archive type.
 *
 *   A representation of a type is defined indepently of any archive type or
 *   any format. Defining a type representation is done by declaring all
 *   attributes needed to construct an object of that type. For example, the
 *   representation of a string could be defined by the knowledge of its size
 *   and its characters.  That way, we can convert a string to any format
 *   without losing information, and getting the string back from any format.
 *   Representation is a conservative declaration of a type that allows
 *   bijective transformations.
 *
 *   A *serializer* is name we use to define a type's representation. Allowing
 *   types to be serialized is done by defining an associated serializer. We
 *   will describe later how to do it in C++.
 *
 * Archive types
 * =============
 *
 *   Each format has an associated archive type that allows to convert objects
 *   from / to that format. For each archive type, we define a pair of archive
 *   types that are exclusively usable as *input* or *output* archive. They
 *   respectively hold an input and an output stream, that contains data in the
 *   archive's format.
 *
 *   For example, base64 serialization can be done through the class
 *   `InputBase64Archive` and `OutputBase64Archive`. Of course, having a
 *   specialized type for input or output serialization implies that you can't
 *   use that same archive for both operations. This constraint add a lot of
 *   safety in the serialization process.
 *
 *   @see elle::serialize::BaseArchive for a description of common behavior of
 *   archive types.
 *
 * Primitive types
 * ===============
 *
 *   Some basic types are handled out of the box by archives. As you might
 *   think, those types do not need to have their serializers.
 *   Supported types:
 *
 *     * All fixed width integer of the form [u]int{8,16,32,64}_t
 *     * `float` and `double` types
 *     * `std::string` and `std::wstring` types
 *     * `bool` type
 *
 *   Note that C strings (`char*` and `wchar_t*`) are not supported, since they
 *   are not considered safe.
 *
 * Types serializer's
 * ==================
 *
 *   User defined types can easily be serialized through any archive type using
 *   a common way to describe their representation: serializers. A type
 *   serializer is defined in a header file, next to the type. Of course,
 *   you'll have to declare a serializer for each type you want to serialize.
 *
 *   The serialization process is checked statically (at compile time), and all
 *   serializers have to be known while serializing any type. This is why they
 *   should be defined in header files. If you forget the serializer, you will
 *   get a compiler error ending with the static assertion "You have to declare
 *   a serializer for this type".
 *
 *   @see elle::serialize::Serializer for more information on serializers.
 *
 * Static Vs dynamic serialization
 * ===============================
 *
 *   As said previously, the serialization process involves only concrete
 *   types. They have to be fully known at compile time, with no polymorphic
 *   behavior at all.
 *   This choice allows the compiler to optimize and inline a lot of calls,
 *   and more importantly, it adds a lot of static checks.
 *
 *   However, it is sometimes required to manipulate, and serialize,
 *   polymorphic types. The way to enable polymorphic serialization is more
 *   intrusive and slower, and should be used only when needed.
 *
 *   Types that are polymorphically serializable use dynamic serialization by
 *   default, unless specified otherwise.
 *
 *   @see elle::serialize::Serializable to enable dynamic serialization.
 *
 * Versionning representation
 * ==========================
 *
 *   Serializer (i.e. representation) are tied to the class they describe. When
 *   the class evolves, so its representation does. But, in many case, you will
 *   want to deserialize objects from a previous representation. In other
 *   words, you surely want to let serialization backward compatible.
 *
 *   Unless you explicitly disable versionning, all user class are versionned,
 *   starting with the version 0. It means that every serialized class are
 *   saved along with a number, the version of the class representation.
 *
 *   When you change a serializer, you might want to increment the
 *   representation version as well. This is the only way you can differentiate
 *   an old representation from a new one.
 *
 *   Static representation version
 *   -----------------------------
 *
 *     The default way to handle representation versionning is to do it
 *     statically. Object are always serialized with the latest version, but
 *     might be loaded from an previous version. That means deserializing an
 *     object (loading it from an archive) is always an *upgrade* process. Even
 *     if the object was stored with an older serializer, it will be restored
 *     as a *new* object. Moreover, there is no way to store an object with an
 *     older representation version than the current one.
 *
 *     Most of the time, this is what you want to do, loading an *old* object,
 *     converting it to a *new* object, and store it back with the current
 *     representation (the newest).
 *
 *     @see ELLE_SERIALIZE_STATIC_REPRESENTATION to declare the representation
 *     version number.
 *
 *   Dynamic representation version
 *   ------------------------------
 *
 *     Upgrading an object from an old representation to the new one could be
 *     tricky, and cannot always be done while deserializing. Dynamic
 *     representation addresses this problem by making the upgrade process
 *     separatly, and only done manually.
 *
 *     An instance of a type  that support dynamic representation versionning
 *     keep it's version number as an attribute. That way, if you save back an
 *     object of any version, it will be saved with the *old* representation.
 *
 *     @see elle::serialize::DynamicRepresentation to enable dynamic
 *     representation.
 *
 *     @see ELLE_SERIALIZE_UGPRADER to define a representation upgrader.
 *
 *   Disable representation versionning
 *   ----------------------------------
 *
 *     If you are obsolutly positive that one object representation will
 *     **never** change, you can disable versionning for its type. You will
 *     save 2 bytes for every stored instance if you do so.
 *
 *     Saving storage can be tempting, but you have to consider that once
 *     disabled, there is no way to enable back representation versionning, or
 *     to load a versionned representation once disabled.
 *
 *     @see ELLE_SERIALIZE_NO_REPRESENTATION to disable representation
 *     versionning.
 *
 */

/**
 * @file    serialize.hh
 * @brief   base toolkit to declare a serializer.
 */
# include "serialize/Serializer.hh"
# include "serialize/Representation.hh"
# include "serialize/StoreRepresentation.hh"
# include "serialize/StaticRepresentation.hh"
# include "serialize/DynamicRepresentation.hh"

#endif
