#include "../State.hh"

namespace surface
{
  namespace gap
  {

    State::SwaggersMap const&
    State::swaggers()
    {
      if (this->_swaggers_dirty)
        {
          auto response = this->_meta->get_swaggers();
          for (auto const& swagger_id: response.swaggers)
            {
              if (this->_swaggers.find(swagger_id) == this->_swaggers.end())
                {
                  auto response = this->_meta->user(swagger_id);
                  this->_swaggers[swagger_id] = new User{
                      response._id,
                      response.fullname,
                      response.email,
                      response.public_key,
                      response.status,
                  };
                }
            }
          this->_swaggers_dirty = false;
        }
      return this->_swaggers;
    }

    User const&
    State::swagger(std::string const& id)
    {
      auto it = this->swaggers().find(id);
      if (it == this->swaggers() .end())
        throw Exception{
            gap_error,
            "Cannot find any swagger for id '" + id + "'"
        };
      return *(it->second);
    }

  }
}

