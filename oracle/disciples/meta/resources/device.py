# -*- encoding: utf-8 -*-

import json
import meta
import web

import metalib

from meta import conf, database
from meta.page import Page

class All(Page):
    """
    Return all user's device ids
        GET /devices
            -> {
                'devices': [device_id1, ...],
            }
    """

    __pattern__ = "/devices"

    def GET(self):
        self.requireLoggedIn()
        return self.success({'devices': self.user.get('devices', [])})


class One(Page):
    """
    Return one user device
        GET /device/id1
            -> {
                '_id': "id",
                'name': "pretty name",
                'passport': "passport string",
            }
    """

    __pattern__ = "/device/(.+)/view"

    def GET(self, id):
        self.requireLoggedIn()
        device = database.devices().find_one({
            '_id': database.ObjectId(id),
            'owner': self.user['_id'],
        })
        device.pop('owner')
        return self.success(device)


class Create(Page):
    """
    Create a new device
        POST /device/create {
            'name': 'pretty name', # required
        }
            -> {
                'success': True,
                'created_device_id': "id",
                'passport': "passport string",
            }
    """
    __pattern__ = "/device/create"

    def POST(self):
        self.requireLoggedIn()
        device = self.data
        name = device.get('name', '').strip()
        if not name:
            return self.error("You have to provide a valid device name")

        to_save = {
            'name': name,
            'owner': self.user['_id'],
        }

        id_ = database.devices().insert(to_save)
        assert id_ is not None

        to_save['passport'] = metalib.generate_passport(
            str(id_),
            conf.INFINIT_AUTHORITY_PATH,
            conf.INFINIT_AUTHORITY_PASSWORD
        )
        database.devices().save(to_save)

        # XXX check unique device ?
        self.user.setdefault('devices', []).append(id_)
        database.users().save(self.user)
        return self.success({
            'created_device_id': id_,
            'passport': to_save['passport']
        })

class Update(Page):
    """
    Update an existing device
        POST /device/update {
            '_id': "id",
            'name': 'pretty name', # optional
        }
            -> {
                'success': True,
                'updated_device_id': "id",
            }
    """

    __pattern__ = "/device/update"

    def POST(self):
        self.requireLoggedIn()
        device = self.data
        assert '_id' in device
        id_ = database.ObjectId(device['_id'].strip())
        if not id_ in self.user['devices']:
            self.forbidden("This network does not belong to you")
        to_save = database.devices().find_one({
            '_id': database.ObjectId(id_)
        })
        if 'name' in device:
            name = device['name'].strip()
            if not name:
                return self.error("You have to provide a valid device name")
            to_save['name'] = name

        id_ = database.devices().save(to_save)
        return self.success({
            'updated_device_id': id_,
            'passport': to_save['passport'],
        })

class Delete(Page):
    """
    Delete a device
        DELETE /device/delete {
            "_id": "device id to delete",
        } -> {
                'success': True,
                'deleted_device_id': "id",
            }
    """

    __pattern__ = "/device/delete"

    def DELETE(self):
        self.requireLoggedIn()
        _id = database.ObjectId(self.data['_id'])
        try:
            devices = self.user['devices']
            idx = devices.index(_id)
            devices.pop(idx)
        except:
            return json.dumps({
                'success': False,
                'error': "The device '%s' was not found" % (_id),
            })
        database.users().save(self.user)
        for network_id in self.user['networks']:
            network = database.networks.find_one(network_id)
            assert network is not None
            try:
                network['devices'].remove(_id)
            except:
                print("Warning: network", network_id, "does not contain any device", _id)
                continue
            database.networks.save(network)

        database.devices().find_and_modify({
            '_id': _id,
            'owner': self.user['_id'], #not required
        }, remove=True)
        return self.success({
            'deleted_device_id': id_,
        })

