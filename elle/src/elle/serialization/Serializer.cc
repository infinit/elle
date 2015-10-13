#include <elle/serialization/Serializer.hh>
#include <elle/serialization/SerializerIn.hh>
#include <elle/serialization/SerializerOut.hh>

namespace elle
{
  namespace serialization
  {
    Serializer::Serializer(bool versioned)
      : _versioned(versioned)
    {}

    bool
    Serializer::_enter(std::string const&)
    {
      return true;
    }

    void
    Serializer::_leave(std::string const&)
    {}

    void
    Serializer::_size(int)
    {}

    bool
    Serializer::in() const
    {
      return !this->_out();
    }

    bool
    Serializer::out() const
    {
      return this->_out();
    }

    bool
    Serializer::_out() const
    {
      return dynamic_cast<SerializerOut const*>(this);
    }

    void
    Serializer::_serialize(std::string const& name, elle::WeakBuffer& v)
    {
      if (this->in())
      {
        elle::Buffer buf;
        this->_serialize(name, buf);
        ELLE_ASSERT_EQ(buf.size(), v.size());
        memcpy(v.mutable_contents(), buf.mutable_contents(), v.size());
      }
      else
      {
        elle::Buffer buf(v.mutable_contents(), v.size());
        this->_serialize(name, buf);
      }
    }

    void
    Serializer::set_context(Context const& context)
    {
      this->_context += context;
    }

    void
    Serializer::_serialize_anonymous(std::string const& name,
                                     std::exception_ptr& e)
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
        e = ExceptionMaker<elle::Exception>::make(*exn);
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
  }
}
