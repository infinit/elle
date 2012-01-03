# -*- encoding: utf-8 -*-

import pymongo

connection = pymongo.Connection('localhost', 27017)

db = connection.meta

# collections
users = db['users']
devices = db['devices']
sessions = db['sessions']


users.ensure_index(
    'email', pymongo.ASCENDING,
    kwags={'unique': True}
)
