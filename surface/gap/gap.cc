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

#define __TO_C(st)    reinterpret_cast<gap_state_t*>(st)
#define __TO_CPP(st)  reinterpret_cast<surface::gap::State*>(st)
// automate cpp wrapping
# define __WRAP_CPP(_state_, _func_, ...)                                     \
    assert(_state_ != nullptr);                                               \
    try                                                                       \
      { __TO_CPP(_state_)->_func_(__VA_ARGS__); }                             \
    catch (std::exception const& err)                                         \
    {                                                                         \
        elle::log::error(#_func_ " error:", err.what());                      \
        return -1;                                                            \
    }                                                                         \
    return 0                                                                  \
  /**/


    gap_state_t* gap_new(char const* updater_path)
    {
      assert(updater_path != nullptr);
      if (elle::Elle::Initialize() == elle::Status::Error ||
          lune::Lune::Initialize() == elle::Status::Error ||
          nucleus::Nucleus::Initialize() == elle::Status::Error)
        {
          elle::log::error("Cannot initialize root components");
          return nullptr;
        }
      return __TO_C(new (std::nothrow) surface::gap::State(updater_path));
    }

    void gap_free(gap_state_t* state)
    {
      delete __TO_CPP(state);
    }

    int gap_refresh_networks(gap_state_t* state)
    {
      __WRAP_CPP(state, refresh_networks);
    }

    char const* gap_path_to_network(gap_state_t* state, char const* path)
    {
      assert(state != nullptr);
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


    int gap_login(gap_state_t* state,
                  char const* email,
                  char const* password)
    {
      __WRAP_CPP(state, login, email, password);
    }

    int gap_register(gap_state_t* state,
                     char const* fullname,
                     char const* email,
                     char const* password)
    {
      __WRAP_CPP(state, register_, fullname, email, password);
    }

    int gap_update_infinit_home(gap_state_t* state)
    {
      __WRAP_CPP(state, update_infinit_home);
    }

    int gap_meta_alive(gap_state_t* state)
    {
      return 1;
    }

} // ! extern "C"
