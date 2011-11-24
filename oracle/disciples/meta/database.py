#
# ---------- imports ----------------------------------------------------------
#

import pymongo

from pymongo import ASCENDING, DESCENDING

#
# ---------- globals ----------------------------------------------------------
#

connection = pymongo.Connection('localhost', 27017)

db = connection.meta

# collections
users = db['users']

devices = db['devices']
