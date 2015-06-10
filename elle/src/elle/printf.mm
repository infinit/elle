#include <elle/printf.hh>

#include <Foundation/Foundation.h>

namespace elle
{
  namespace _details
  {
    static
    bool
    _was_nil(boost::format& fmt, id value)
    {
      if (value == nullptr)
      {
        fmt % "<nil>";
        return true;
      }
      return false;
    }

    void
    feed(boost::format& fmt, NSArray* value)
    {
      if (!_was_nil(fmt, value))
        feed(fmt, value.description);
    }

    void
    feed(boost::format& fmt, NSDictionary* value)
    {
      if (!_was_nil(fmt, value))
        feed(fmt, value.description);
    }

    void
    feed(boost::format& fmt, NSError* value)
    {
      if (!_was_nil(fmt, value))
        feed(fmt, value.description);
    }

    void
    feed(boost::format& fmt, NSNumber* value)
    {
      if (!_was_nil(fmt, value))
        feed(fmt, value.stringValue);
    }

    void
    feed(boost::format& fmt, NSString* value)
    {
      if (!_was_nil(fmt, value))
        fmt % (value.length ? value.UTF8String : "<empty>");
    }
  }
}
