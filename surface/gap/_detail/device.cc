#include "../State.hh"

#include <common/common.hh>

#include <elle/Passport.hh>

ELLE_LOG_COMPONENT("infinit.surface.gap.State");

namespace surface
{
  namespace gap
  {

    namespace fs = boost::filesystem;
    namespace json = elle::format::json;

    bool
    State::has_device() const
    {
      ELLE_ASSERT(this->_me._id.size() > 0 && "not properly initialized");
      ELLE_DEBUG("Check for '%s' device existence at '%s'",
                 this->_me._id,
                 common::passport_path(this->_me._id));
      return fs::exists(common::passport_path(this->_me._id));
    }

    std::string const&
    State::device_id()
    {
      if (this->_device_id.size() == 0)
        {
          elle::Passport passport;
          passport.load(elle::io::Path{common::passport_path(this->_me._id)});
          this->_device_id = passport.id();
          this->_device_name = passport.name();
        }
      return this->_device_id;
    }

    std::string const&
    State::device_name()
    {
      if (this->_device_name.size() == 0)
        {
          elle::Passport passport;
          passport.load(elle::io::Path{common::passport_path(this->_me._id)});
          this->_device_id = passport.id();
          this->_device_name = passport.name();
        }
      return this->_device_name;
    }

    void
    State::update_device(std::string const& name, bool force_create)
    {
      std::string passport_path = common::passport_path(this->_me._id);

      this->_device_name = name;
      ELLE_DEBUG("Device to update: '%s'", this->_device_name.c_str());

      std::string passport_string;
      if (force_create || !this->has_device())
        {
          auto res = this->_meta->create_device(name);
          passport_string = res.passport;
          this->_device_id = res.created_device_id;
          ELLE_DEBUG("Created device id: %s", this->_device_id.c_str());
        }
      else
        {
          ELLE_DEBUG("Loading passport from '%s'.", passport_path);
          elle::Passport passport;
          passport.load(elle::io::Path{passport_path});

          ELLE_DEBUG("Passport id: %s", passport.id());
          auto res = this->_meta->update_device(passport.id(), name);
          this->_device_id = res.updated_device_id;
          passport_string = res.passport;
        }

      elle::Passport passport;
      if (passport.Restore(passport_string) == elle::Status::Error)
        throw Exception(gap_wrong_passport, "Cannot load the passport");

      passport.store(elle::io::Path(passport_path));
    }

  }
}

