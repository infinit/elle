#include <cassert>
#include <cstdlib>

#include <elle/log.hh>
#include <elle/Elle.hh>
#include <lune/Lune.hh>
#include <nucleus/Nucleus.hh>

#include <elle/idiom/Close.hh>

#include "gap.h"
#include "State.hh"

extern "C"
{

#define __TO_C(st)    reinterpret_cast<gap_State*>(st)
#define __TO_CPP(st)  reinterpret_cast<surface::gap::State*>(st)
// automate cpp wrapping
# define __WRAP_CPP_RET(_state_, _func_, ...)                                 \
    assert(_state_ != nullptr);                                               \
    gap_Status ret;                                                           \
    try                                                                       \
      { __TO_CPP(_state_)->_func_(__VA_ARGS__); ret = gap_ok; }               \
    catch (std::exception const& err)                                         \
    {                                                                         \
        elle::log::error(#_func_ " error:", err.what());                      \
        ret = gap_error;                                                      \
    }                                                                         \
  /**/
# define __WRAP_CPP(...)                                                      \
    __WRAP_CPP_RET(__VA_ARGS__);                                              \
    return ret                                                                \
  /**/


    gap_State* gap_new()
    {
      static bool initialized = false;
      if (!initialized)
        {
          initialized = true;
          if (elle::Elle::Initialize() == elle::Status::Error ||
              lune::Lune::Initialize() == elle::Status::Error ||
              nucleus::Nucleus::Initialize() == elle::Status::Error)
            {
#include <elle/idiom/Open.hh>
              show();
#include <elle/idiom/Close.hh>
              elle::log::error("Cannot initialize root components");
              return nullptr;
            }
        }

      try
        {
          return __TO_C(new surface::gap::State());
        }
      catch (std::exception const& err)
        {
          elle::log::error("Cannot initialize gap state:", err.what());
          return nullptr;
        }
    }

    void gap_free(gap_State* state)
    {
      delete __TO_CPP(state);
    }

    gap_Status gap_refresh_networks(gap_State* state)
    {
      __WRAP_CPP(state, refresh_networks);
    }

    char const* gap_path_to_network(gap_State* state, char const* path)
    {
      assert(state != nullptr);
      assert(path != nullptr);
      try
        {
          std::string const& network = __TO_CPP(state)->path_to_network(path);
          return network.c_str();
        }
      catch (std::exception const& err)
        {
          elle::log::warn("Cannot convert path", path, "to network:", err.what());
        }
      return nullptr;
    };


    gap_Status gap_login(gap_State* state,
                         char const* email,
                         char const* password)
    {
      assert(email != nullptr);
      assert(password != nullptr);
      __WRAP_CPP(state, login, email, password);
    }

    gap_Status gap_register(gap_State* state,
                            char const* fullname,
                            char const* email,
                            char const* password,
                            char const* device_name)
    {
      assert(fullname != nullptr);
      assert(email != nullptr);
      assert(password != nullptr);
      __WRAP_CPP_RET(state, register_, fullname, email, password);
      if (ret == gap_ok && device_name != nullptr)
        return gap_set_device_name(state, device_name);
      return ret;
    }

    gap_Status gap_meta_alive(gap_State* state)
    {
      return gap_ok;
    }

    gap_Status gap_set_device_name(gap_State* state,
                                   char const* name)
    {
      assert(name != nullptr);
      __WRAP_CPP(state, update_device, name);
    }

} // ! extern "C"
