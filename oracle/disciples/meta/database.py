# -*- encoding: utf-8 -*-

import pymongo
import pymongo.objectid

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

def ObjectId(_id): return pymongo.objectid.ObjectId(_id)

# functions
def byId(collection, _id):
    """
    Get an object from collection `collection' with its id `_id'
    """
    return collection.find_one({'_id': pymongo.objectid.ObjectId(_id)})
