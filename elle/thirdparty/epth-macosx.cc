//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu feb  2 22:07:26 2012]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/thirdparty/epth.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

//
// ---------- globals ---------------------------------------------------------
//

static ::pth_attr_t    epth_attribute;

//
// ---------- functions -------------------------------------------------------
//

///
/// this function initializes the epth wrapper.
///
elle::radix::Status     epth_initialize(const elle::core::Natural32     size,
                                        t_epth&                         program)
{
  //
  // initialize the Pth library.
  //

  if (::pth_init() == FALSE)
    escape("unable to initialize the Pth library");

  //
  // allocate and set the epth attribute for cooperative threads to
  // be spawn the right way i.e not joinable and with an appropriate
  // stack size.
  //

  if ((epth_attribute = ::pth_attr_new()) == NULL)
    escape("unable to initialize the Pth library");

  if (::pth_attr_set(epth_attribute,
                     ::PTH_ATTR_STACK_SIZE,
                     size) == -1)
    escape("unable to set the Pth stack size");

  //
  // retrieve the main fiber's context.
  //

  if ((program = ::pth_self()) == NULL)
    escape("unable to retrieve the program's thread");

  return elle::radix::StatusOk;
}

///
/// this function cleans the library.
///
elle::radix::Status     epth_clean()
{
  //
  // destroy the pth attribute.
  //

  if (::pth_attr_destroy(epth_attribute) == -1)
    escape("unable to destroy the Pth attribtue");

  //
  // clean and stop the Pth library.
  //

  if (::pth_kill() == FALSE)
    escape("unable to kill the Pth library");

  return elle::radix::StatusOk;
}

///
/// this function spawns a new thread by starting executing
/// the code at the given function, with the given argument
/// i.e _function_(_argument_);
///
elle::radix::Status     epth_spawn(void*                (*function)(void*),
                                   void*                argument,
                                   t_epth&              context)
{
  //
  // spawn the new thread and set the fiber's context.
  //
  // note that a stack will be automatically allocated so that
  // the fiber->frame object becomes useless.
  //

  if ((context = ::pth_spawn(epth_attribute,
                             function,
                             argument)) == NULL)
    escape("unable to spawn the thread");

  return elle::radix::StatusOk;
}

///
/// this function switches fibers, saving _from_'s context and
/// restoring _to_'s.
///
elle::radix::Status     epth_switch(const t_epth&,
                                    const t_epth&               to)
{
  //
  // yield execution to the next thread (in Epth's terms).
  //
  // this call will automatically save the current thread.
  //

  if (::pth_yield(to) == -1)
    escape("unable to yield execution");

  return elle::radix::StatusOk;
}

///
/// this function aborts a thread, forcing it to exit.
///
elle::radix::Status     epth_abort(const t_epth&                id)
{
  //
  // abort the thread.
  //

  if (::pth_abort(id) == -1)
    escape("unable to abort the thread");

  return elle::radix::StatusOk;
}
