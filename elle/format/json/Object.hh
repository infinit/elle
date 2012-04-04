#ifndef ELLE_FORMAT_JSON_OBJECT_HH
# define ELLE_FORMAT_JSON_OBJECT_HH

# include <cstdint>
# include <memory>
# include <string>

# include "elle/serialize/JSONArchive.fwd.hh"

namespace elle { namespace format { namespace json {

    /// Root object for all json types
    class Object
    {
    public:
      /// JSON archive may access to Save() method
      typedef elle::serialize::OutputJSONArchive OutputJSONArchive;
      typedef elle::serialize::InputJSONArchive  InputJSONArchive;
      friend class elle::serialize::OutputJSONArchive;
      friend class elle::serialize::InputJSONArchive;

    public:
      virtual ~Object() {}

    public:
      virtual void Save(elle::serialize::OutputJSONArchive& ar) const = 0;
      virtual std::unique_ptr<Object> Clone() const = 0;

      std::string repr() const;
    };

}}}

#endif /* ! OBJECT_HH */


