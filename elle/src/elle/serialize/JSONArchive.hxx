#ifndef ELLE_SERIALIZE_JSONARCHIVE_HXX
# define ELLE_SERIALIZE_JSONARCHIVE_HXX

# include "JSONArchive.hh"

# include <elle/format/json.hh>

namespace elle
{
  namespace serialize
  {
    namespace json = elle::format::json;

    template<ArchiveMode mode>
    class JSONArchive:
      public BaseArchive<mode, JSONArchive<mode>>
    {
    private:
      typedef BaseArchive<mode, JSONArchive<mode>> BaseClass;
      typedef typename BaseClass::StreamType StreamType;
      friend class BaseClass::Access;

      struct Kind
      {
        enum {array, dict, none} kind;
        bool is_first;
      };
      std::vector<Kind> _save_stack;

    public:
      JSONArchive(StreamType& stream);
      JSONArchive(StreamType& stream,
                  json::Object&& obj);
      JSONArchive(StreamType& stream,
                  json::Object const& obj);

      template <typename T>
      JSONArchive(StreamType& stream,
                  T&& obj):
        BaseClass(stream)
      {
        *this & std::forward<T>(obj);
      }

    protected:
      void Save(uint16_t val);
      void Save(uint32_t val);
      void Save(uint64_t val);
      void Save(int16_t val);
      void Save(int32_t val);
      void Save(int64_t val);
      void Save(float val);
      void Save(double val);
      void Save(std::string const& val);

      template<typename T> inline typename std::enable_if<
          std::is_base_of<json::Object, T>::value
      >::type Save(T const& value)
      {
        value.repr(this->stream());
      }

      template<typename T> inline typename std::enable_if<
          !std::is_base_of<json::Object, T>::value &&
          !std::is_arithmetic<T>::value &&
          !std::is_same<T, std::string>::value
      >::type Save(T const& val)
      {
        if (this->_save_stack.size() == 0)
          this->_save_stack.push_back({Kind::none, true});
        bool pushed = false;
        if (this->_save_stack.back().kind == Kind::none)
        {
          pushed = true;
          this->_save_stack.push_back({Kind::dict, true});
          this->stream() << "{";
        }
        auto& back = this->_save_stack.back();
        if (!pushed && back.kind == Kind::dict)
        {
          if (!back.is_first)
          {
            this->stream() << ',';
            back.is_first = true;
          }
        }

        typedef Serializer<typename std::remove_cv<T>::type> Serializer;
        Serializer::Serialize(
            *this,
            const_cast<T&>(val),
            0
        );

        if (pushed)
        {
          ELLE_ASSERT(this->_save_stack.size() != 0);
          this->stream() << "}";
          this->_save_stack.pop_back();
        }
        else
          back.is_first = false;
      }

      template <typename T>
      inline
      void
      Save(Concrete<T> const& concrete)
      {
        ELLE_ASSERT(this->_save_stack.size() != 0);
        this->_save_stack.push_back(this->_save_stack.back());
        this->Save(concrete.value);
        ELLE_ASSERT(this->_save_stack.size() != 0);
        this->_save_stack.pop_back();
        ELLE_ASSERT(this->_save_stack.size() != 0);
        this->_save_stack.back().is_first = false;
      }

      template <typename T>
      inline
      void
      Save(NamedValue<T> const& pair)
      {
        if (this->_save_stack.back().is_first)
          this->_save_stack.back().is_first = false;
        else
          this->stream() << ',';
        (*this) << pair.name;
        this->stream() << ':';
        this->_save_stack.push_back({Kind::none, true});
        (*this) << pair.value;
        this->_save_stack.pop_back();
      }

      friend class json::detail::BasicObject<int32_t>;
      friend class json::detail::BasicObject<bool>;
      friend class json::detail::BasicObject<double>;
      friend class json::detail::BasicObject<std::string>;
      friend class json::Null;
      friend class json::Array;
      friend class json::Dictionary;

    private:
      std::vector<json::Object const*> _load_stack;
    protected:
      template<typename T>
      typename std::enable_if<!json::detail::ObjectCanLoad<T>::value>::type
      Load(T& val)
      {
        bool pushed = false;
        std::unique_ptr<json::Object> root;
        if (this->_load_stack.size() == 0)
        {
          auto parser = json::Parser<std::string>();
          root = parser.parse(this->stream());
          this->_load_stack.push_back(root.get());
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
          this->_load_stack.pop_back();
        }
      }

      template<typename T>
      typename std::enable_if<json::detail::ObjectCanLoad<T>::value>::type
      Load(T& val)
      {
        ELLE_ASSERT(this->_load_stack.size() != 0);
        this->_load_stack.back()->load(val);
      }

      template <typename T>
      void
      Load(Concrete<T> const& val)
      {
        BaseClass::Load(val);
      }

      template <typename T>
      void
      Load(NamedValue<T> const& pair)
      {
        ELLE_ASSERT(this->_load_stack.size() != 0);
        json::Object const* object =
          this->_load_stack.back()->as_dictionary().contains(pair.name);
        ELLE_ASSERT(object != nullptr); // XXX KeyError
        this->_load_stack.push_back(object);
        this->Load(pair.value);
        ELLE_ASSERT(this->_load_stack.size() != 0);
        this->_load_stack.pop_back();
      }
    };
  }
}

#endif
