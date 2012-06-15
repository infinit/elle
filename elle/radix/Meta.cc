//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [mon apr 26 20:08:34 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/system/Platform.hh>

#include <elle/radix/Meta.hh>

#include <elle/standalone/Report.hh>
#include <elle/standalone/Maid.hh>
#include <elle/standalone/Log.hh>

#include <elle/io/Path.hh>
#include <elle/io/File.hh>
#include <elle/io/Directory.hh>

namespace elle
{
  using namespace system;

  using namespace standalone;
  using namespace io;

  namespace radix
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this value defines if the meta debugging must be activated.
    ///
    /// note that this configuration parameter cannot be put elsewhere
    /// as everything relies on Meta.
    ///
    /// this value can obviously be changed to true to activate debugging.
    ///
    const Boolean               Meta::Debug::Status = true;

    ///
    /// this value states if the debugging has been started or not.
    ///
    /// this variable should always be set to false at the beginning.
    ///
    Boolean                     Meta::Debug::State = false;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the meta.
    ///
    Status              Meta::Initialize()
    {
      // if the debug has been activated.
      if (Meta::Debug::Status == true)
        {
#if defined(INFINIT_LINUX)
          // initialize the traces.
          if (Trace::Initialize() == Status::Error)
            escape("unable to initialize the trace system");
#endif

          // enable the logging.
          if (Meta::Enable() == Status::Error)
            escape("unable to enable the meta logging");
        }

      return Status::Ok;
    }

    ///
    /// this method cleans the meta.
    ///
    Status              Meta::Clean()
    {
      // if traces exist, dump them.
      if (Meta::Debug::Status == true)
        {
          // disable the logging.
          if (Meta::Disable() == Status::Error)
            escape("unable to disable the meta logging");

          // show the traces.
          if (Meta::Show() == Status::Error)
            escape("unable to show the meta");

#if defined(INFINIT_LINUX)
          // clean the traces.
          if (Trace::Clean() == Status::Error)
            escape("unable to clean the trace system");
#endif
        }

      return Status::Ok;
    }

    ///
    /// this method enables the allocation logging.
    ///
    Status              Meta::Enable()
    {
      // set the state.
      Meta::Debug::State = true;

      return Status::Ok;
    }

    ///
    /// this method disables the allocation logging.
    ///
    Status              Meta::Disable()
    {
      // set the state.
      Meta::Debug::State = false;

      return Status::Ok;
    }

    ///
    /// this method dumps all the traces.
    ///
    Status              Meta::Show(const Natural32              margin)
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Meta]" << std::endl;

      // if traces exist, dump them.
      if (Meta::Debug::Status == true)
        {
#if defined(INFINIT_LINUX)
          // show the traces.
          if (Trace::Show(margin + 2) == Status::Error)
            escape("unable to show the traces");
#endif
        }

      return Status::Ok;
    }

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// destructor.
    ///
    Meta::~Meta()
    {
    }

//
// ---------- operators -------------------------------------------------------
//

    ///
    /// this operator allocates memory.
    ///
    /// note that since new operators cannot return NULL, all the errors
    /// handling is skipped.
    ///
    void*               Meta::operator new(size_t               size)
    {
      void*             address;

      // allocate memory on the heap
      address = ::malloc(size);

      // only proceed if debugging has been activated.
      if ((Meta::Debug::Status == true) &&
          (Meta::Debug::State == true))
        {
#if defined(INFINIT_LINUX)
          // store the trace.
          if (Trace::Store(address) == Status::Error)
            {
              log("unable to store the trace for %p",
                  address);

              goto _corps;
            }
#endif
        }

    _corps:
      return (address);
    }

    ///
    /// this operator is called whenever an object is allocated in
    /// a given memory area referenced by _address_.
    ///
    void*               Meta::operator new(size_t,
                                           void*                address)
    {
      // return the address of the already reserved memory area.
      return (address);
    }

    ///
    /// this operator allocates memory for an array.
    ///
    /// note that since new operators cannot return NULL, all the errors
    /// handling is skipped.
    ///
    void*               Meta::operator new[](size_t             size)
    {
      void*             address;

      // allocate memory on the heap
      address = ::malloc(size);

      // only proceed if debugging has been activated.
      if ((Meta::Debug::Status == true) &&
          (Meta::Debug::State == true))
        {
#if defined(INFINIT_LINUX)
          // store the trace.
          if (Trace::Store(address) == Status::Error)
            {
              log("unable to store the trace for %p",
                  address);

              goto _corps;
            }
#endif
        }

    _corps:
      return (address);
    }

    ///
    /// this operator is called whenever an object is allocated in
    /// a given memory area referenced by _address_.
    ///
    void*               Meta::operator new[](size_t,
                                             void*              address)
    {
      // return the address of the already reserved memory area.
      return (address);
    }

    ///
    /// this method releases previously-reserved memory.
    ///
    void                Meta::operator delete(void*             address)
    {
      // only proceed if debugging has been activated.
      if ((Meta::Debug::Status == true) &&
          (Meta::Debug::State == true))
        {
#if defined(INFINIT_LINUX)
          // erase the trace.
          if (Trace::Erase(address) == Status::Error)
            {
              log("unable to erase the trace for %p",
                  address);

              goto _corps;
            }
#endif
        }

    _corps:

      // release the memory anyway.
      ::free(address);
    }

  }
}
