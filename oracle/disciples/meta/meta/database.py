# -*- encoding: utf-8 -*-

import pymongo
import pymongo.objectid

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
    return collection.find_one({'_id': pymongo.objectid.ObjectId(_id)})
