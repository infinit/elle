# -*- encoding: utf-8 -*-

import pymongo.objectid
import json

from meta import database
from meta.page import Page

class Device(Page):
    """
    Ability to
        GET device ids, or one specific device
        POST new one
        DELETE one
    """

    def GET(self, id=None):
        self.requireLoggedIn()
        if id is None:
            return json.dumps(self.user.get('devices', []))
        else:
            return json.dumps(database.devices.find_one({
                '_id': pymongo.objectid.ObjectId(id),
                'owner': self.user['_id'],
            }), default=str)

    def POST(self):
        """
        XXX: add check for valid names, and valid ip
        XXX: make the creation of the device and the user update only one transaction
        """
        self.requireLoggedIn()
        i = self.input
        name = i.get('name', '').strip()
        ip_address = i.get('ip_address', '').strip()
        if not name:
            return json.dumps({
                'success': False,
                'error': "You have to provide a valid device name",
            })
        if not ip_address:
            return json.dumps({
                'success': False,
                'error': "You have to provide a valid device ip address",
            })
        device = {
            'name': name,
            'ip_address': ip_address,
            'owner': self.user['_id'], #usefull ?
        }
        if '_id' in i:
            if i['_id'] not in self.user['devices']:
                raise web.Forbidden("This device does not belong to you")
            device['_id'] = pymongo.objectid.ObjectId(i['_id'])
            id = database.devices.save(device)
        else:
            id = database.devices.insert(device)
            self.user.setdefault('devices', []).append(str(id))
            database.users.save(self.user)

        return json.dumps({
            'success': True,
            'device_id': str(id),
        })

    def DELETE(self, id):
        self.requireLoggedIn()
        try:
            devices = self.user['devices']
            idx = devices.index(id)
            print "devices left:", id, idx, devices
            devices.pop(idx)
            print "devices left:", id, idx, devices
        except:
            return json.dumps({
                'success': False,
                'error': "The device '%s' was not found" % (id),
            })
        database.users.save(self.user)
        database.devices.find_and_modify({
            '_id': pymongo.objectid.ObjectId(id),
            'owner': self.user['_id'], #not required
        }, remove=True)
        return json.dumps({
            'success': True,
            'removed_device_id': id,
        })

