#ifndef JSON_SPIRIT_PATH_ERROR
#define JSON_SPIRIT_PATH_ERROR

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <string>
#include "platform.h"

namespace json_spirit {

/** A PathError is thrown when you try to access an element by path, e.g. by
 *  calling Value::getObject("foo.bar.baz") and there is some error in accessing
 *  the path: one of the values on the path isn't an object or doesn't
 *  exist. This can also be thrown on Value::insert() or Value::put() calls if
 *  you specify a path that contains a non-object.
 */
template<typename StringT>
struct BasicPathError {
    BasicPathError( const StringT& path_, const StringT& element_, const std::string& reason_ = std::string("") )
     : path(path_), element(element_), reason(reason_)
    {}

    bool operator==(const BasicPathError& rhs) const {
        return (path == rhs.path &&
            element == rhs.element);
        // Ignore the reason since the strings could change -- we really care
        // that the path and element are identical.
    }

    StringT path;
    StringT element;
    std::string reason;
};

}

#endif
