#ifndef  ELLE_SERIALIZE_UNIQUABLE_HXX
# define ELLE_SERIALIZE_UNIQUABLE_HXX

# include <sstream>

# include <elle/serialize/Base64Archive.hh>

# include <elle/serialize/Uniquable.hh>

# include <elle/standalone/Log.hh>
# include <elle/standalone/Report.hh>
# include <elle/idiom/Open.hh>

namespace elle
{
  namespace serialize
  {

    template<typename T>
      Status Uniquable<T>::Save(Unique& out) const
      {
        T const& self = *this;
        std::ostringstream ss;
        try
          {
            elle::serialize::OutputBase64Archive(ss, self);
          }
        catch (std::exception const& err)
          {
            escape(err.what());
          }

        out = ss.str();
        return Status::Ok;
      }

    template<typename T>
      Status Uniquable<T>::Restore(Unique const& in)
      {
        T& self = static_cast<T&>(*this);
        std::istringstream ss(in);
        try
          {
            elle::serialize::InputBase64Archive archive(ss);
            archive >> self;
          }
        catch (std::exception const& err)
          {
            escape(err.what());
          }

        return Status::Ok;
      }

  }
}


#endif

