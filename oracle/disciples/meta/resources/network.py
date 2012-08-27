# -*- encoding: utf-8 -*-

import json
import traceback
import web

import metalib

import meta.mail
from meta.page import Page
from meta import database, conf

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
            print self.user
            print network
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

NETWORK_INVITATION_NEWUSER_SUBJECT = u"%(added_by)s wants you to join the '%(network_name)s' network on Infinit!"
NETWORK_INVITATION_NEWUSER_CONTENT = u"""
Hi %(recipient)s,

%(added_by)s has added you to the '%(network_name)s' network on Infinit.
Infinit is an incredibly powerful tool that lets you create folders with the
members of your different communities, giving you the ability to easily share
and access files and folders collaboratively.

You’ll no longer have to worry about storage limits in the cloud, large email
attachments or downloading files again!
And best of all, it's completely free :)

Click the link below to download Infinit and access your new network!

Download here: http://infinit.io/download

All the best,

--%(space)s
The Infinit Team
infinit.io
""".strip()

NETWORK_INVITATION_SUBJECT = u"%(added_by)s has just added you to the '%(network_name)s' network on Infinit!"
NETWORK_INVITATION_CONTENT = u"""
Hi %(recipient)s,

%(added_by)s has added you to the '%(network_name)s' network. Just launch
Infinit to access it!

Networks are made up of your community’s combined storage space allowing
everyone to access more content than they can store on their own devices. The
more space you and your community add to a network, the more files and folders,
you can all access.

Sharing and accessing files has never been easier! And best of all, it's
completely free :)

All the best,

--%(space)s
The Infinit Team
infinit.io
""".strip()

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

    def POST(self):
        to_add_user_id = database.ObjectId(self.data['user_id'])
        network = self.network(self.data['_id'])
        if network['owner'] != self.user['_id']:
            raise web.Forbidden("You cannot add a user in a network that does "
                                "not belong to you")
        #XXX users should invited instead of added
        if to_add_user_id in network['users']:
            return self.error("This user is already in the network")
        to_add_user = database.byId(database.users(), to_add_user_id)
        if '@' in to_add_user['email']:
            infos = {
                'added_by': self.user['fullname'],
                'recipient': to_add_user['fullname'],
                'network_name': network['name'],
                'space': ' ',
            }
            subject = NETWORK_INVITATION_SUBJECT % infos
            content = NETWORK_INVITATION_CONTENT % infos
            meta.mail.send(to_add_user['email'], subject, content)
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
        addrs = {'local': [], 'external': []}
        for node in network['nodes'].values():
            print(node)
            for addr_kind in ['local', 'external']:
                addr = node[addr_kind]
                if addr and addr[0] and addr[1]:
                    print("Append", addr_kind, addr)
                    addrs[addr_kind].append(
                        addr[0] + ':' + str(addr[1]),
                    )
        res['nodes'] = addrs['local'] + addrs['external']
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

    __pattern__ = "/network/update"

    def POST(self):
        self.requireLoggedIn()
        for k in ['devices', 'users']:
            if k in self.data:
                return self.error("key %s is deprecated" % k)

        _id = database.ObjectId(self.data['_id'])
        to_save = self.network(_id)

        if 'name' in self.data:
            name = self.data['name'].strip()
            try:
                self._check_name(name, _id)
            except Exception, e:
                return self.error(str(e))
            to_save['name'] = name

        if 'root_block' in self.data and self.data['root_block'] and \
           'root_address' in self.data and self.data['root_address'] and \
           'access_block' in self.data and self.data['access_block'] and \
           'access_address' in self.data and self.data['access_address'] and \
           'group_block' in self.data and self.data['group_block'] and \
           'group_address' in self.data and self.data['group_address']:
            if to_save['root_block'] is not None:
                return self.error("This network has already a root block")

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
                    return self.error("The root block was not properly signed")

                to_save['root_block'] = root_block
                to_save['root_address'] = root_address
                to_save['access_block'] = access_block
                to_save['access_address'] = access_address
                to_save['group_block'] = group_block
                to_save['group_address'] = group_address

                to_save['descriptor'] = metalib.generate_network_descriptor(
                    str(_id),
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
                return self.error("Unexpected error: " + str(err))

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

    def POST(self):
        # XXX What are security check requirement ?
        self.requireLoggedIn()
        network_id = database.ObjectId(self.data["_id"])
        device_id = database.ObjectId(self.data["device_id"])
        network = database.networks().find_one(network_id)
        if not network:
            return self.error("Network not found.")
        device = database.devices().find_one(device_id)
        if not device:
            return self.error("Device not found.")
        if str(device_id) not in network['nodes']:
            network['nodes'][str(device_id)] = {
                    "local": None,
                    "external": None,
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
        "local": (192.168.x.x, 62014),

        # optional external address and port
        "external": (212.27.23.67, 62015)
    }
        -> {
            "updated_network_id": "the same network id",
        }

    """
    __pattern__ = '/network/connect_device'

    def POST(self):
        self.requireLoggedIn()
        network_id = database.ObjectId(self.data["_id"])
        device_id = database.ObjectId(self.data["device_id"])
        network = database.networks().find_one(network_id)
        if not network:
            return self.error("Network not found.")
        device = database.devices().find_one(device_id)
        if not device:
            return self.error("Device not found.")
        node = network['nodes'].get(str(device_id))
        if node is None:
            return self.error("Cannot find the device in this network")
        local_address = self.data.get('local')
        if local_address is not None:
            node['local'] = (local_address[0], int(local_address[1]))
        external_address = self.data.get('external')
        if external_address is not None:
            node['external'] = (external_address[0], int(external_address[1]))
        else:
            node['external'] = (web.ctx.env['REMOTE_ADDR'], node['local'] and node['local'][1] or 0)
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

    def POST(self):
        if '_id' in self.data:
            return self.error("An id cannot be specified while creating a self.data")
        name = self.data.get('name', '').strip()
        try:
            self._check_name(name)
        except Exception, e:
            return self.error(str(e))

        for k in ['devices', 'users']:
            if k in self.data:
                return self.error("The key %s is deprecated" % k)
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

    def DELETE(self):
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

