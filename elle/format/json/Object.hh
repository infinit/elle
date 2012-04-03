#ifndef OBJECT_HH
# define OBJECT_HH

# include <cstdint>
# include <memory>

# include "elle/serialize/JSONArchive.fwd.hh"

namespace elle { namespace format { namespace json {

    /// Root object for all json types
    struct Object
    {
      /// JSON archive may access to Save() method
      friend class OutputJSONArchive;
      friend class InputJSONArchive;

    public:
      virtual ~Object() {}

    public:
      virtual void Save(OutputJSONArchive& ar) const = 0;
      virtual std::unique_ptr<Object> Clone() const = 0;

      std::string repr() const;
    };

}}}

#endif /* ! OBJECT_HH */


