# -*- encoding: utf-8 -*-

import json
import pymongo.objectid

from meta.page import Page
from meta import database

class Network(Page):
    """
    Return all user's network ids
        GET /networks
            -> [network_id1, ...]

    Return one user network
        GET /network/id1
            -> {
                '_id': "id",
                'name': "pretty name",
                'devices': [device_id1, ...],
                'users': [user_id1, ...]
            }

    Create a new network
        POST /network {
            'name': 'pretty name', # required
            'users': [user_id1, ...], # optional
            'devices': [device_id1, ...], # optional
        }
            -> {
                'success': True,
                'created_network_id': "id",
            }

    Update an existing network
        POST /network {
            '_id': "id",
            'name': 'pretty name', # optional
            'users': [user_id1, ...], # optional
            'devices': [device_id1, ...], # optional
        }
            -> {
                'success': True,
                'updated_network_id': "id",
            }

    Delete a network
        DELETE /network/id
            -> {
                'success': True,
                'deleted_network_id': "id",
            }
    """

    def GET(self, id=None):
        self.requireLoggedIn()
        if id is None:
            return json.dumps(self.user.get('networks', []))
        else:
            network = database.networks.find_one({
                '_id': pymongo.objectid.ObjectId(id),
                'owner': self.user['_id'],
            })
            network.pop('owner')
            return json.dumps(network, default=str)

    def POST(self):
        self.requireLoggedIn()
        network = self.data
        if '_id' in network:
            action = "update"
        else:
            action = "create"

        action_func = {
            'create': self._create,
            'update': self._update,
        }.get(action.lower())

        if action_func is None:
            return self.error("Unknown action '%s'" % str(action))
        return json.dumps(action_func(network))

    def _create(self, network):
        if '_id' in network:
            return self.error("An id cannot be specified while creating a network")
        name = network.get('name', '').strip()
        if not self._checkName(name):
            return self.error("You have to provide a valid network name")
        devices = filter(self._checkDevice, map(lambda d: d.strip(), network.get('devices', [])))
        users = filter(self._checkUser, map(lambda d: d.strip(), network.get('users', [])))

        network = {
            'name': name,
            'owner': self.user['_id'],
            'users': users,
            'devices': devices,
        }
        id = database.networks.insert(network)
        assert id is not None
        self.user.setdefault('networks', []).append(str(id))
        database.users.save(self.user)
        return {
            'success': True,
            'created_network_id': str(id)
        }

    def _update(self, network):
        id = str(network['_id']).strip()
        if id not in self.user['networks']:
            raise web.Forbidden("This network does not belong to you")
        id = pymongo.objectid.ObjectId(id)
        to_save = database.networks.find_one({'_id': id})
        if 'name' in network:
            name = network['name'].strip()
            if not self._checkName(name):
                return self.error("Given network name is not valid")
            to_save['name'] = name
        if 'devices' in network:
            devices = filter(self._checkDevice, map(lambda d: d.strip(), network.get('devices', [])))
            to_save['devices'] = devices

        if 'users' in network:
            users = filter(self._checkUser, map(lambda d: d.strip(), network.get('users', [])))
            to_save['users'] = users

        id = database.networks.save(to_save)
        return {
            'success': True,
            'updated_network_id': str(id)
        }

    def _checkName(self, name):
        return bool(name)
    def _checkDevice(self, device_id):
        """
        device_id is not empty and belongs to the user
        """
        if not device_id:
            return False
        return device_id in self.user.devices

    def _checkUser(self, user_id):
        """
        user_id is not empty and
        is different from the owner and
        exists
        """
        if not user_id:
            return False
        if user_id == str(self.user['_id']):
            return False
        return database.users.find_one({
            '_id': pymongo.objectid.ObjectId(user_id),
        }) is not None

    def DELETE(self, id):
        self.requireLoggedIn()
        try:
            networks = self.user['devices']
            idx = networks.index(id)
            networks.pop(idx)
        except:
            return json.dumps({
                'success': False,
                'error': "The network '%s' was not found" % (id),
            })
        database.users.save(self.user)
        database.networks.find_and_modify({
            '_id': pymongo.objectid.ObjectId(id),
            'owner': self.user['_id'], #not required
        }, remove=True)
        return json.dumps({
            'success': True,
            'deleted_network_id': id,
        })

