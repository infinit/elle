#include <elle/serialization/Serializer.hh>
#include <elle/serialization/SerializerIn.hh>
#include <elle/serialization/SerializerOut.hh>

ELLE_LOG_COMPONENT("elle.serialization.Serializer");

namespace elle
{
  namespace serialization
  {
    Serializer::Serializer(bool versioned)
      : _versioned(versioned)
    {
      static_assert(Details::api<int>() == Details::pod, "");
      static_assert(Details::api<unsigned long>() == Details::pod, "");
    }

    Serializer::Serializer(Versions versions, bool versioned)
      : _versioned(versioned)
      , _versions(std::move(versions))
    {}

    /*--------------.
    | Enter / leave |
    `--------------*/

    Serializer::Entry::Entry(Serializer& s, std::string const& name)
      : _serializer(s)
      , _log(ELLE_LOG_VALUE(
               elle::log::Logger::Level::trace,
               elle::log::Logger::Type::info,
               "%s: %sserialize \"%s\"", s, s.in() ? "de" : "", name))
      , _name(name)
      , _entered(this->_serializer._enter(name))
    {
      if (this->_entered)
        s._names.emplace_back(name);
    }

    Serializer::Entry::~Entry()
    {
      if (this->_entered)
      {
        this->_serializer._names.pop_back();
        this->_serializer._leave(this->_name);
      }
    }

    Serializer::Entry::operator bool() const
    {
      return this->_entered;
    }

    Serializer::Entry
    Serializer::enter(std::string const& name)
    {
      return Entry(*this, name);
    }

    bool
    Serializer::_enter(std::string const&)
    {
      return true;
    }

    void
    Serializer::_leave(std::string const&)
    {}

    std::string
    Serializer::current_name() const
    {
      return this->_names.empty() ? "" : this->_names.back();
    }

    void
    Serializer::_size(int)
    {}

    bool
    Serializer::text() const
    {
      return this->_text();
    }

    bool
    Serializer::_text() const
    {
      return true;
    }

    bool
    Serializer::in() const
    {
      return !this->out();
    }

    void
    Serializer::_serialize(elle::WeakBuffer& v)
    {
      if (this->in())
      {
        auto buf = elle::Buffer{};
        this->_serialize(buf);
        ELLE_ASSERT_EQ(buf.size(), v.size());
        // FIXME: why an actual copy?
        memcpy(v.mutable_contents(), buf.mutable_contents(), v.size());
      }
      else
      {
        auto buf = elle::Buffer(v);
        this->_serialize(buf);
      }
    }

    void
    Serializer::set_context(Context const& context)
    {
      this->_context += context;
    }

    void
    Serializer::_serialize_anonymous_exception(std::exception_ptr& e)
    {
      if (this->out())
        try
        {
          std::rethrow_exception(e);
        }
        catch (elle::Exception& e)
        {
          auto ptr = &e;
          this->serialize("exception", ptr);
        }
        catch (std::exception const& e)
        {
          ELLE_ABORT("not handled yet");
        }
        catch (...)
        {
          ELLE_ABORT("not handled yet");
        }
      else
      {
        std::unique_ptr<elle::Exception> exn;
        this->serialize("exception", exn);
        e = ExceptionMaker<elle::Exception>::make(std::move(*exn));
      }
    }

    void
    Serializer::_serialize_dict_key(std::string const&,
                                    std::function<void ()> const&)
    {
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      ELLE_WARN("%s: do nothing", *this);
    }

    void
    Serializer::_deserialize_dict_key(
      std::function<void (std::string const&)> const&)
    {
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      ELLE_WARN("%s: do nothing", *this);
    }

    void
    Serializer::serialize_variant(std::vector<std::string> const& names,
                        int index, // out: filled, in: -1
                        std::function<void(int)> const& f)
    {
      this->_serialize_variant(names, index, f);
    }

    void
    Serializer::_serialize_variant(std::vector<std::string> const& names,
                                  int index, // out: filled, in: -1
                                  std::function<void(int)> const& f)
    {
      this->serialize("type", index);
      this->_enter("value");
      elle::SafeFinally leave([&] { this->_leave("value");});
      f(index);
    }
  }
}
