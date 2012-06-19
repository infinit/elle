# -*- encoding: utf-8 -*-

import pymongo
try:
    from pymongo.objectid import ObjectId
except ImportError:
    from bson.objectid import ObjectId

"""
Define database collections and their constraints
"""

connection = pymongo.Connection('localhost', 27017)

db = connection.meta

# collections
users = db['users']
devices = db['devices']
sessions = db['sessions']
networks = db['networks']

# constraints
users.ensure_index(
    'email', pymongo.ASCENDING,
    kwags={'unique': True}
)

# functions
def byId(collection, _id):
    """
    Get an object from collection `collection' with its id `_id'
    """
    return collection.find_one({'_id': ObjectId(_id)})
