#
# ---------- imports ----------------------------------------------------------
#

import web
import json

from meta import database

#
# ---------- classes ----------------------------------------------------------
#

class User:
  def GET(self, name):
    user = database.users.find_one({'name': name})

    return templetor.user(user)

  def DELETE(self, name):
    print "DELETE " + name

    return """
{
     "firstName": "John",
     "lastName" : "Smith",
     "age"      : 25,
     "address"  :
     {
         "streetAddress": "21 2nd Street",
         "city"         : "New York",
         "state"        : "NY",
         "postalCode"   : "10021"
     },
     "phoneNumber":
     [
         {
           "type"  : "home",
           "number": "212 555-1234"
         },
         {
           "type"  : "fax",
           "number": "646 555-4567"
         }
     ]
}
"""
    return json.dumps(['suce', 'foo', {'bar': ('baz', None, 1.0, 2)}])
