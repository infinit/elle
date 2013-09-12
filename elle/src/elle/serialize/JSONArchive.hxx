#ifndef ELLE_SERIALIZE_JSONARCHIVE_HXX
# define ELLE_SERIALIZE_JSONARCHIVE_HXX

# include "JSONArchive.hh"

# include <elle/finally.hh>
# include <elle/format/json/Dictionary.hh>
# include <elle/format/json/Parser.hh>

namespace elle
{
  namespace serialize
  {
    namespace json = elle::format::json;

    template <ArchiveMode mode>
    void
    JSONArchive<mode>::_push_save(Kind const kind)
    {
      this->_save_stack.push_back({kind, true});
      if (kind == Kind::dict)
        this->stream() << '{';
      else if (kind == Kind::array)
        this->stream() << '[';
    }

    template <ArchiveMode mode>
    void
    JSONArchive<mode>::_pop_save(Kind const kind)
    {
      ELLE_ASSERT_GT(this->_save_stack.size(), 0u);
      ELLE_ASSERT_EQ(this->_save_stack.back().kind, kind);
      this->_save_stack.pop_back();
      if (kind == Kind::dict)
        this->stream() << '}';
      else if (kind == Kind::array)
        this->stream() << ']';
      if (this->_save_stack.size() > 0)
        this->_save_stack.back().is_first = false;
    }

    template <ArchiveMode mode>
    void
    JSONArchive<mode>::_elem_save()
    {
      if (this->_save_stack.size() == 0)
        return;
      if (this->_save_stack.back().is_first)
        this->_save_stack.back().is_first = false;
      else
        this->stream() << ',';
    }


    template <ArchiveMode mode>
    template <typename T>
    JSONArchive<mode>::JSONArchive(StreamType& stream,
                                   T&& obj):
      BaseClass(stream)
    {
      *this & std::forward<T>(obj);
    }

    template <ArchiveMode mode>
    template <typename T>
    typename std::enable_if<std::is_enum<T>::value>::type
    JSONArchive<mode>::Save(T const value)
    {
      this->_elem_save();
      this->Save(static_cast<int32_t>(value));
    }

    template <ArchiveMode mode>
    template <typename T>
    inline
    typename std::enable_if<std::is_base_of<json::Object, T>::value>::type
    JSONArchive<mode>::Save(T const& value)
    {
      this->_elem_save();
      value.repr(this->stream());
    }

    template <ArchiveMode mode>
    template <typename T>
    inline
    typename std::enable_if<
      !std::is_base_of<json::Object, T>::value &&
    !std::is_arithmetic<T>::value &&
    !std::is_same<T, std::string>::value &&
    !std::is_enum<T>::value &&
    !elle::format::json::detail::IsArray<T>::value
    >::type
    JSONArchive<mode>::Save(T const& val)
    {
      bool is_base_class =  (
        this->_save_stack.size() > 0 &&
        this->_save_stack.back().kind == Kind::base_class
        );
      if (!is_base_class)
      {
        this->_elem_save();
        this->_push_save(Kind::dict);
      }
      typedef Serializer<typename std::remove_cv<T>::type> Serializer;
      Serializer::Serialize(
        *this,
        const_cast<T&>(val),
        0
        );
      if (!is_base_class)
        this->_pop_save(Kind::dict);
    }

    template <ArchiveMode mode>
    template <typename T>
    inline
    typename std::enable_if<
      elle::format::json::detail::IsArray<T>::value
      >::type
    JSONArchive<mode>::Save(T& value)
    {
      this->_push_save(Kind::array);
      for (auto& el: value)
      {
        this->_elem_save();
        this->_push_save(Kind::none);
        *this & el;
        this->_pop_save(Kind::none);
      }
      this->_pop_save(Kind::array);
    }

    template <ArchiveMode mode>
    template <typename T>
    inline
    void
    JSONArchive<mode>::Save(Concrete<T> const& concrete)
    {
      this->_push_save(Kind::base_class);
      this->Save(concrete.value);
      this->_pop_save(Kind::base_class);
    }

    template <ArchiveMode mode>
    template <typename T>
    inline
    void
    JSONArchive<mode>::Save(NamedValue<T> const& pair)
    {
      this->_elem_save();
      this->_push_save(Kind::none);
      (*this) << pair.name;
      this->_pop_save(Kind::none);
      this->stream() << ':';
      this->_push_save(Kind::none);
      (*this) << pair.value;
      this->_pop_save(Kind::none);
    }

    template <ArchiveMode mode>
    template<typename T>
    typename std::enable_if<!json::detail::ObjectCanLoad<T>::value>::type
    JSONArchive<mode>::Load(T& val)
    {
      bool pushed = false;
      if (this->_load_stack.size() == 0)
      {
        auto parser = json::Parser<std::string>();
        auto root = parser.parse(this->stream());
        this->_load_stack.push_back(root.get());
        root.release();
        pushed = true;
      }
      auto& back = this->_load_stack.back();
      ELLE_ASSERT(back != nullptr);

      typedef Serializer<typename std::remove_cv<T>::type> Serializer;
      Serializer::Serialize(
        *this,
        const_cast<T&>(val),
        0
        );
      if (pushed)
      {
        ELLE_ASSERT(this->_load_stack.size() != 0);
        elle::SafeFinally pop_back([this] () { this->_load_stack.pop_back(); });
        // According to the documentation, JSONObject destructors don't throw.
        delete this->_load_stack.back();
      }
    }

    template <ArchiveMode mode>
    template<typename T>
    typename std::enable_if<json::detail::ObjectCanLoad<T>::value>::type
    JSONArchive<mode>::Load(T& val)
    {
      ELLE_ASSERT(this->_load_stack.size() != 0);
      this->_load_stack.back()->load(val);
    }

    template <ArchiveMode mode>
    template <typename T>
    void
    JSONArchive<mode>::Load(Concrete<T> const& val)
    {
      BaseClass::Load(val);
    }

    template <ArchiveMode mode>
    template <typename T>
    void
    JSONArchive<mode>::Load(NamedValue<T> const& pair)
    {
      ELLE_ASSERT(this->_load_stack.size() != 0);
      json::Object const* object =
        this->_load_stack.back()->as_dictionary().contains(pair.name);
      if (object == nullptr)
        throw elle::Exception{
          "Cannot find any key '" + pair.name + "' in '" +
            this->_load_stack.back()->repr() + "'"};
      this->_load_stack.push_back(object);
      try
      {
        this->Load(pair.value);
        ELLE_ASSERT(this->_load_stack.size() != 0);
        this->_load_stack.pop_back();
      }
      catch (...)
      {
        this->_load_stack.pop_back();
        throw;
      }
    }
  }
}

#endif
