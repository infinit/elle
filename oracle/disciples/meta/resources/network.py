# -*- encoding: utf-8 -*-

import json
import traceback
import web

import metalib

from meta.page import Page
from meta import database, conf

class _Page(Page):
    """Common tools for network calls."""

    def network(self, _id):
        self.requireLoggedIn()
        network = database.networks().find_one({
            '_id': database.ObjectId(_id),
            'owner': database.ObjectId(self.user['_id']),
        })
        if not network:
            raise web.Forbidden("This network does not belong to you")
        return network

    def _check_name(self, name):
        """name is not empty"""
        return bool(name)

    def _check_device(self, device_id):
        """device_id is not empty and belongs to the user"""
        if not device_id:
            return False
        return device_id in self.user['devices']

    def _check_user(self, user_id):
        """user_id is not empty and is different from the owner and exists"""
        if not user_id:
            return False
        if user_id == str(self.user['_id']):
            return False
        return database.users().find_one({
            '_id': database.ObjectId(user_id),
        }) is not None

    def _unique_ids_check(self, ids, checker):
        ids = filter(checker, map(lambda d: database.ObjectId(d.strip()), ids))

class AddUser(_Page):
    """
    Add a user in a network
        POST {
            "user_id": "id of the user to add",
        }
            -> {
                "updated_network_id": "id1",
            }
    """

    __pattern__ = "/network/add_user"

    def POST(self, _id):
        to_add_user_id = database.ObjectId(self.data['user_id'])
        network = self.network(_id)
        #XXX users should invited instead of added
        if to_add_user_id in network['users']:
            return self.error("This user is already in the network")
        network['users'].append(to_add_user_id)
        database.networks().save(network)
        return self.success({
            'updated_network_id': _id,
        })

class All(_Page):
    """
    Return all user's network ids
        GET /networks
            -> {
                'networks': [network_id1, ...]
            }
    """

    __pattern__ = "/networks"

    def GET(self):
        self.requireLoggedIn()
        return self.success({'networks': self.user.get('networks', [])})

class One(_Page):
    """
    Return one user network
        GET /network/id1
            -> {
                '_id': "id",
                'name': "pretty name",
                'model': 'slug',
                'devices': [device_id1, ...],
                'users': [user_id1, ...],
                'root_block': "base64 string of the root block"
                'descriptor': "base64 string of descriptor",
            }
    """

    __pattern__ = "/network/(.+)"

    def GET(self, _id):
        print("Get one network: %s" % _id)
        network = self.network(_id)
        network.pop('owner')
        return self.success(network)

class Nodes(Page):
    """
    Return connected nodes of a network
        GET /network/id1/nodes
            -> {
                'success': True,
                'network_id': 'id1',
                'nodes': [ "ip:port", "ip2:port", ...],
            }
    """
    __pattern__ = "/network/(.+)/nodes"

    def GET(self, _id):
        network = self.network(_id)
        res = {
            "network_id": network['_id'],
            "nodes": [],
        }
        for device_id in network['devices']:
            device = database.byId(database.devices(), device_id)
            if device:
                for addr_kind in ['local_address', 'extern_address']:
                    addr = device[addr_kind]
                    if addr['port']:
                        res['nodes'].append(
                            addr['ip'] + ':' + str(addr['port']),
                        )
            else:
                print "No more device_id", device['_id']
        return self.success(res)


class Update(_Page):
    """
    Update an existing network
        POST /network {
            '_id': "id",
            'name': 'pretty name', # optional
            'users': [user_id1, ...], # optional
            'devices': [device_id1, ...], # optional
            'root_block': "base64 root block", # optional (implies root_address)
            'root_address': "base64 root address", # optional (implies root_block)
        }
            -> {
                'success': True,
                'updated_network_id': "id",
                'descriptor' : "descriptor file", # only if root_block and root_address where given
                'root_block': "base64 root block", # only if root_block and root_address where given

            }
    """

    __pattern__ = "/network/update"

    def POST(self):
        self.requireLoggedIn()
        self.data = self.data
        id_ = database.ObjectId(self.data['_id'])
        to_save = self.network(id_)

        if 'name' in self.data:
            name = self.data['name'].strip()
            if not self._check_name(name):
                return self.error("Given network name is not valid")
            to_save['name'] = name

        if 'devices' in self.data:
            to_save['devices'] = self._unique_ids_check(
                self.data.get('devices', []),
                self._check_device
            )

        if 'users' in self.data:
            to_save['users'] = self._unique_ids_check(
                self.data.get('users', []),
                self._check_user
            )

        if 'root_block' in self.data and 'root_address' in self.data and \
           self.data['root_block'] and self.data['root_address']:
            if to_save['root_block'] is not None:
                return self.error("This network has already a root block")

            try:
                root_block = self.data['root_block']
                root_address = self.data['root_address']
                assert(self.data.get('descriptor') is None)
                is_valid = metalib.check_root_block_signature(
                    root_block,
                    root_address,
                    self.user['public_key']
                )
                if not is_valid:
                    return self.error("The root block was not properly signed")

                to_save['root_block'] = root_block
                to_save['root_address'] = root_address

                to_save['descriptor'] = metalib.generate_network_descriptor(
                    str(id),
                    to_save['name'],
                    to_save.get('model', 'slug'),
                    root_address,
                    conf.INFINIT_AUTHORITY_PATH,
                    conf.INFINIT_AUTHORITY_PASSWORD,
                )
                print("Generated descriptor !")
            except Exception, err:
                traceback.print_exc()
                return self.error("Unexpected error: " + str(err))

        id_ = database.networks().save(to_save)
        res = {
            'updated_network_id': id_
        }
        if 'descriptor' in to_save:
            res['descriptor'] = to_save['descriptor']
            res['root_block'] = to_save['root_block']

        return self.success(res)


class Create(_Page):
    """
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
    """

    __pattern__ = "/network/create"

    def POST(self):
        if '_id' in self.data:
            return self.error("An id cannot be specified while creating a self.data")
        name = self.data.get('name', '').strip()
        if not self._check_name(name):
            return self.error("You have to provide a valid self.data name")
        devices = filter(
            self._check_device,
            map(
                lambda d: database.ObjectId(d.strip()),
                self.data.get('devices', [])
            )
        )
        users = filter(
            self._check_user,
            map(
                lambda d: database.ObjectId(d.strip()),
                self.data.get('users', [])
            )
        )
        print(self.user)
        print(self.user['_id'], database.ObjectId(self.user['_id']))
        network = {
            'name': name,
            'owner': self.user['_id'],
            'model': 'slug',
            'users': users,
            'devices': devices,
            'descriptor': None,
            'root_block': None,
            'root_address': None,
        }
        _id = database.networks().insert(network)
        assert _id is not None
        self.user.setdefault('networks', []).append(_id)
        database.users().save(self.user)
        return self.success({
            'created_network_id': _id
        })


class Delete(_Page):
    """
    Delete a network
        DELETE /network/id
            -> {
                'success': True,
                'deleted_network_id': "id",
            }
    """

    __pattern__ = "/network/(.+)"

    def DELETE(self, _id):
        self.requireLoggedIn()
        _id = database.ObjectId(_id)
        try:
            networks = self.user['devices']
            idx = networks.index(_id)
            networks.pop(idx)
        except:
            return self.error("The network '%s' was not found" % str(_id))
        database.users().save(self.user)
        database.networks().find_and_modify({
            '_id': _id,
            'owner': self.user['_id'], #not required
        }, remove=True)
        return  self.success({
            'deleted_network_id': _id,
        })

