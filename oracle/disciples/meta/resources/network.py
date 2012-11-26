# -*- encoding: utf-8 -*-

import json
import traceback
import web

import metalib

from meta import mail
from meta.page import Page
from meta import database, conf
from meta import error
from meta import regexp


class _Page(Page):
    """Common tools for network calls."""

    def network(self, _id):
        self.requireLoggedIn()
        network = database.networks().find_one({
            '_id': database.ObjectId(_id),
        })
        if not network:
            raise web.NotFound("Couldn't find any network with this id")
        if network['owner'] != self.user['_id'] and \
           self.user['_id'] not in network['users']:
            raise web.Forbidden("This network does not belong to you")
        return network

    def _check_name(self, name, id_=None):
        """name is not empty and does not already exists for current user."""
        assert id_ is None or isinstance(id_, database.ObjectId)
        if not name:
            raise Exception("Invalid network name")
        req = {
            'owner': database.ObjectId(self.user['_id']),
            'name': name,
        }
        if id_ is None:
            n = database.networks().find_one(req)
        else:
            n = database.networks().find_one(req, {'_id': id_})
        if n is not None:
            raise Exception("Cannot have two network with the same name!")

    def _check_device(self, device_id):
        """device_id is not empty and belongs to the user"""
        if not device_id:
            return False
        return database.ObjectId(device_id) in self.user['devices']

    def _check_user(self, user_id):
        """user_id is not empty and is different from the owner and exists"""
        if not user_id:
            return False
        if str(user_id) == str(self.user['_id']):
            return False
        return database.users().find_one({
            '_id': database.ObjectId(user_id),
        }) is not None

    def _unique_ids_check(self, ids, checker):
        return list(set(filter(
            checker,
            map(lambda d: database.ObjectId(str(d).strip()), ids)
        )))

class AddUser(_Page):
    """
    Add a user in a network
        POST {
            "_id": "network id",
            "user_id": "id of the user to add",
        }
            -> {
                "updated_network_id": "id1",
            }
    """

    __pattern__ = "/network/add_user"

    _validators = {
        '_id': regexp.Validator(regexp.ID, error.NETWORK_ID_NOT_VALID),
        'user_id': regexp.Validator(regexp.ID, error.USER_ID_NOT_VALID),
    }

    def POST(self):

        status = self.validate()
        if status:
            return self.error(*status)

        to_add_user_id = database.ObjectId(self.data['user_id'])
        network = self.network(self.data['_id'])
        if network['owner'] != self.user['_id']:
            raise web.Forbidden("You cannot add a user in a network that does "
                                "not belong to you")
        #XXX users should invited instead of added
        if to_add_user_id in network['users']:
            return self.error(error.ALREADY_LOGGED_IN)
        to_add_user = database.byId(database.users(), to_add_user_id)
        if '@' in to_add_user['email']:
            infos = {
                'added_by': self.user['fullname'],
                'recipient': to_add_user['fullname'],
                'network_name': network['name'],
                'space': ' ',
            }
            subject = mail.NETWORK_INVITATION_SUBJECT % infos
            content = mail.NETWORK_INVITATION_CONTENT % infos
            mail.send(to_add_user['email'], subject, content)
        network['users'].append(to_add_user_id)
        database.networks().save(network)
        to_add_user['networks'].append(network['_id'])
        database.users().save(to_add_user)
        return self.success({
            'updated_network_id': network['_id'],
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
        GET
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

    __pattern__ = "/network/(.+)/view"

    def GET(self, _id):
        network = self.network(_id)
        return self.success(network)

class Nodes(_Page):
    """
    Return connected nodes of a network
        GET
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
        addrs = {'locals': [], 'externals': []}
        for node in network['nodes'].values():
            for addr_kind in ['locals', 'externals']:
                addr = node[addr_kind]
                if addr and addr["ip"] and addr["port"]:
                    print("Append", addr_kind, addr)
                    addrs[addr_kind].append(
                        addr["ip"] + ':' + str(addr["port"]),
                    )
        res['nodes'] = addrs['locals'] + addrs['externals']
        print("Find nodes of %s: " % network['name'], res['nodes'])
        return self.success(res)


class Update(_Page):
    """
    Update an existing network
        POST {
            '_id': "id",
            'name': 'pretty name', # optional
            'users': [user_id1, ...], # DEPRECATED
            'devices': [device_id1, ...], # DEPRECATED

            'root_block': "base64 root block", # optional (implies root_address)
            'root_address': "base64 root address", # optional (implies root_block)
            'access_block': "base64 access block", # optional (implies access_address)
            'access_address': "base64 access address", # optional (implies access_block)
            'group_block': "base64 group block", # optional (implies group_address)
            'group_address': "base64 group address", # optional (implies group_block)
        }
            -> {
                'success': True,
                'updated_network_id': "id",
            }
    """

    _validators = {
        '_id': regexp.Validator(regexp.ID, error.NETWORK_ID_NOT_VALID)
    }

    __pattern__ = "/network/update"

    def POST(self):
        self.requireLoggedIn()
        for k in ['devices', 'users']:
            if k in self.data:
                return self.error(error.DEPRECATED, "Key %s is deprecated." % k)

        status = self.validate()
        if status:
            return self.error(*status)

        _id = database.ObjectId(self.data['_id'])
        to_save = self.network(_id)

        if 'name' in self.data:
            name = self.data['name'].strip()
            try:
                self._check_name(name, _id)
            except Exception, e:
                return self.error(error.UNKNOWN, str(e))
            to_save['name'] = name

        if 'root_block' in self.data and self.data['root_block'] and \
           'root_address' in self.data and self.data['root_address'] and \
           'access_block' in self.data and self.data['access_block'] and \
           'access_address' in self.data and self.data['access_address'] and \
           'group_block' in self.data and self.data['group_block'] and \
           'group_address' in self.data and self.data['group_address']:
            if to_save['root_block'] is not None:
                return self.error(error.ROOT_BLOCK_ALREADY_EXIST)

            try:
                root_block = self.data['root_block']
                root_address = self.data['root_address']
                access_block = self.data['access_block']
                access_address = self.data['access_address']
                group_block = self.data['group_block']
                group_address = self.data['group_address']

                assert self.data.get('descriptor') is None
                is_valid = metalib.check_root_directory_signature(
                    root_block,
                    root_address,
                    access_block,
                    access_address,
                    group_block,
                    group_address,
                    self.user['public_key']
                )
                if not is_valid:
                    return self.error(error.ROOT_BLOCK_BADLY_SIGNED)

                to_save['root_block'] = root_block
                to_save['root_address'] = root_address
                to_save['access_block'] = access_block
                to_save['access_address'] = access_address
                to_save['group_block'] = group_block
                to_save['group_address'] = group_address

                to_save['descriptor'] = metalib.generate_network_descriptor(
                    str(_id),
                    self.user['identity'],
                    to_save['name'],
                    to_save.get('model', 'slug'),
                    root_address,
                    group_address,
                    conf.INFINIT_AUTHORITY_PATH,
                    conf.INFINIT_AUTHORITY_PASSWORD,
                )
                print("Generated descriptor !")
            except Exception, err:
                traceback.print_exc()
                return self.error(error.UNKNOWN, "Unexpected error: " + str(err))

        _id = database.networks().save(to_save)
        return self.success({
            'updated_network_id': _id
        })


class AddDevice(_Page):
    """
    Add a device to a network
        POST {
            '_id': "id",
            "device_id": "device id",
        }
            -> {
                'success': True,
                'updated_network_id': "id",
            }
    """
    __pattern__ = '/network/add_device'

    _validators = {
        '_id': regexp.Validator(regexp.ID, error.NETWORK_ID_NOT_VALID),
        'device_id': regexp.Validator(regexp.DeviceID, error.DEVICE_ID_NOT_VALID),
    }

    def POST(self):
        # XXX What are security check requirement ?
        self.requireLoggedIn()

        status = self.validate()
        if status:
            return self.error(*status)

        network_id = database.ObjectId(self.data["_id"])
        device_id = database.ObjectId(self.data["device_id"])
        network = database.networks().find_one(network_id)
        if not network:
            return self.error(error.NETWORK_NOT_FOUND)
        device = database.devices().find_one(device_id)
        if not device:
            return self.error(error.DEVICE_NOT_FOUND)
        if str(device_id) not in network['nodes']:
            network['nodes'][str(device_id)] = {
                    "locals": None,
                    "externals": None,
            }
        database.networks().save(network)
        return self.success({
            "updated_network_id": network['_id'],
        })

class ConnectDevice(_Page):
    """
    Connect the device to a network (setting ip and port)
    POST {
        "_id": "the network id",
        "device_id": "the device id",

        # Optional local ip, port
        "locals": [{"ip" : 192.168.x.x, "port" : 62014}, ...],

        # optional external address and port
        "externals": [{"ip" : "212.27.23.67", "port" : 62015}, ...],
    }
        -> {
            "updated_network_id": "the same network id",
        }
    """
    __pattern__ = '/network/connect_device'

    _validators = {
        '_id': regexp.Validator(regexp.ID, error.NETWORK_ID_NOT_VALID),
        'device_id': regexp.Validator(regexp.DeviceID, error.DEVICE_ID_NOT_VALID),
    }

    def POST(self):
        self.requireLoggedIn()

        status = self.validate()
        if status:
            return self.error(*status)

        network_id = database.ObjectId(self.data["_id"])
        device_id = database.ObjectId(self.data["device_id"])

        network = database.networks().find_one(network_id)
        if not network:
            return self.error(error.NETWORK_NOT_FOUND)

        device = database.devices().find_one(device_id)
        if not device:
            return self.error(error.DEVICE_NOT_FOUND)

        node = network['nodes'].get(str(device_id))
        if node is None:
            return self.error(error.DEVICE_NOT_IN_NETWORK)

        local_addresses = self.data.get('locals') # notice the 's'
        if local_addresses is not None:
            # Generate a list of dictionary ip:port.
            # We can not take the local_addresses content directly:
            # it's not checked before this point. Therefor, it's insecure.
            node['locals'] = [{"ip" : v["ip"], "port" : v["port"]} for v in local_addresses]

        external_addresses = self.data.get('externals')
        if external_addresses is not None:
            node['externals'] = [{"ip" : v["ip"], "port" : v["port"]} for v in external_addresses]
        else:
            node['externals'] = []

        database.networks().save(network)

        print("Connected device", device['name'], "(%s)" % device_id,
              "to network", network['name'], "(%s)" % network_id,
              "with", node)
        return self.success({
            "updated_network_id": network_id
        })

class Create(_Page):
    """
    Create a new network
        POST  {
            'name': 'pretty name', # required
            'users': [user_id1, ...], # DEPRECATED
            'devices': [device_id1, ...], # DEPRECATED
        }
            -> {
                'success': True,
                'created_network_id': "id",
            }
    """

    __pattern__ = "/network/create"

    _validators = {
        'name': regexp.Validator(regexp.NotNull, error.DEVICE_NOT_VALID),
    }

    def POST(self):
        self.requireLoggedIn()

        status = self.validate()
        if status:
            return self.error(*status)

        name = self.data.get('name', '').strip()
        try:
            self._check_name(name)
        except Exception, e:
            return self.error(error.UNKNOWN, str(e))

        for k in ['devices', 'users']:
            if k in self.data:
                return self.error(error.DEPRECATED, "The key %s is deprecated" % k)

        users =[]
        if self.user['_id'] not in users:
            assert isinstance(self.user['_id'], database.ObjectId)
            users.append(self.user['_id'])

        network = {
            'name': name,
            'owner': self.user['_id'],
            'model': 'slug',
            'users': [self.user['_id']],
            'nodes': {},
            'descriptor': None,
            'root_block': None,
            'root_address': None,
            'access_block': None,
            'access_address': None,
            'group_block': None,
            'group_address': None,
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
        DELETE {
            '_id': "id",
        } -> {
                'success': True,
                'deleted_network_id': "id",
            }
    """

    __pattern__ = "/network/delete"

    _validators = {
        '_id': regexp.Validator(regexp.NetworkID, error.NETWORK_ID_NOT_VALID)
    }

    def DELETE(self):
        self.requireLoggedIn()

        _id = database.ObjectId(_id)
        try:
            networks = self.user['devices']
            idx = networks.index(_id)
            networks.pop(idx)
        except:
            return self.error(error.NETWORK_NOT_FOUND, "The network '%s' was not found" % str(_id))
        database.users().save(self.user)
        database.networks().find_and_modify({
            '_id': _id,
            'owner': self.user['_id'], #not required
        }, remove=True)
        return  self.success({
            'deleted_network_id': _id,
        })
