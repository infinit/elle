# -*- encoding: utf-8 -*-

import pymongo
try:
    from pymongo.objectid import ObjectId
except ImportError:
    from bson.objectid import ObjectId

"""
Define database collections and their constraints
"""

_connection = None
def connection(host="localhost", port=27017):
    global _connection
    if _connection is None:
        _connection = pymongo.Connection(host, port)
    return _connection

_database = None
def database(conn=None):
    global _database
    if conn is not None:
        return conn.meta
    if _database is None:
        _database = connection().meta
    return _database

# collections
_users = None
_devices = None
_sessions = None
_networks = None

def users(conn=None):
    global _users
    if _users is None:
        _users = database(conn)['users']
        # constraints
        _users.ensure_index(
            'email', pymongo.ASCENDING,
            kwags={'unique': True}
        )
    return _users

def devices(conn=None):
    global _devices
    if _devices is None:
        _devices = database(conn)['devices']
    return _devices

def sessions(conn=None):
    global _sessions
    if _sessions is None:
        _sessions = database(conn)['sessions']
    return _sessions

def networks(conn=None):
    global _networks
    if _networks is None:
        _networks = database(conn)['networks']
    return _networks

# functions
def byId(collection, _id):
    """
    Get an object from collection `collection' with its id `_id'
    """
    return collection.find_one({'_id': ObjectId(_id)})
