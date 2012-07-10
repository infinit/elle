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

# define CATCH_ALL(_func_)                                                    \
    catch (plasma::meta::Exception const& err)                                \
    {                                                                         \
        elle::log::error(#_func_ " error:", err.what());                      \
        if (err.code == plasma::meta::Error::network_error)                   \
          ret = gap_network_error;                                            \
        else                                                                  \
          ret = gap_internal_error;                                           \
    }                                                                         \
    catch (surface::gap::Exception const& err)                                \
    {                                                                         \
      elle::log::error(#_func_ " error:", err.what());                        \
      ret = err.code;                                                         \
    }                                                                         \
    catch (std::exception const& err)                                         \
    {                                                                         \
        elle::log::error(#_func_ " error:", err.what());                      \
        ret = gap_internal_error;                                             \
    }                                                                         \
  /**/

// automate cpp wrapping
# define __WRAP_CPP_RET(_state_, _func_, ...)                                 \
    assert(_state_ != nullptr);                                               \
    gap_Status ret;                                                           \
    try                                                                       \
      { __TO_CPP(_state_)->_func_(__VA_ARGS__); ret = gap_ok; }               \
    CATCH_ALL(_func_)                                                         \
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

    gap_Status gap_meta_status(gap_State* state)
    {
      return gap_ok;
    }

    gap_Status gap_refresh_networks(gap_State* state)
    {
      __WRAP_CPP(state, refresh_networks);
    }

    char* gap_hash_password(gap_State* state,
                            char const* email,
                            char const* password)
    {
      assert(state != nullptr);
      assert(email != nullptr);
      assert(password != nullptr);

      try
      {
        std::string h = __TO_CPP(state)->hash_password(email, password);
        return ::strdup(h.c_str());
      }
      catch (std::exception const& err)
      {
        elle::log::error("Couldn't hash the password:", err.what());
      }
      return nullptr;
    }

    void gap_hash_free(char* h)
    {
      ::free(h);
    }

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
        {
          __WRAP_CPP(state, update_device, device_name, true);
        }
      return ret;
    }

    gap_Status gap_set_device_name(gap_State* state,
                                   char const* name)
    {
      assert(name != nullptr);
      __WRAP_CPP(state, update_device, name);
    }

    /// Retrieve all user networks.
    char** gap_networks(gap_State* state)
    {
      assert(state != nullptr);
      gap_Status ret = gap_ok;
      try
        {
          auto const& networks_map = __TO_CPP(state)->networks();

          // compute total size needed
          size_t total_size = 0;
          for (auto const& network_pair: networks_map)
            total_size += network_pair.first.size() + 1 + sizeof(char*);
          total_size += sizeof(char*);

          void* ptr = malloc(total_size);
          if (ptr == nullptr)
            return nullptr;

          // the memory area contains an array of c string pointers, and
          // strings themselves are stored right after that.
          char** networks = reinterpret_cast<char**>(ptr);
          char* network_str = reinterpret_cast<char*>(networks + (networks_map.size() + 1));
          for (auto const& network_pair: networks_map)
            {
              char const* str = network_pair.first.c_str();
              size_t size = network_pair.first.size();

              memcpy(network_str, str, size);
              network_str[size] = '\0';
              *networks = network_str;
              networks++;
              network_str += size + 1;
            }
          *networks = nullptr; // guard

          return reinterpret_cast<char**>(ptr); // original start pointer
        }
      CATCH_ALL(networks);

      (void) ret;
      return nullptr;
    }

    void gap_networks_free(char** networks)
    {
      ::free(networks);
    }

    gap_Status gap_create_network(gap_State* state,
                                  char const* name)
    {
      __WRAP_CPP(state, create_network, name);
    }

    gap_Status gap_launch_watchdog(gap_State* state)
    {
      __WRAP_CPP(state, launch_watchdog);
    }

    gap_Status gap_stop_watchdog(gap_State* state)
    {
      __WRAP_CPP(state, stop_watchdog);
    }

    gap_Status gap_set_permissions(gap_State* state,
                                   char const* user_id,
                                   char const* absolute_path,
                                   int permissions)
    {
      assert(user_id != nullptr);
      assert(absolute_path != nullptr);
      __WRAP_CPP(state, set_permissions, user_id, absolute_path, permissions);
    }
} // ! extern "C"
