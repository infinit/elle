# -*- encoding: utf-8 -*-

import json

import metalib

from meta import conf, database
from meta.page import Page

class Device(Page):
    """
    Return all user's device ids
        GET /devices
            -> {
                'devices': [device_id1, ...],
            }

    Return one user device
        GET /device/id1
            -> {
                '_id': "id",
                'name': "pretty name",
                'addresses': [
                    {'ip': 'ip address 1', 'port': 1912},
                    ...
                ],
                'passport': "passport string",
            }

    Create a new device
        POST /device {
            'name': 'pretty name', # required
            'addresses': [
                {'ip': 'ip address 1', 'port': 1912},
                ...
            ],
        }
            -> {
                'success': True,
                'created_device_id': "id",
                'passport': "passport string",
            }

    Update an existing device
        POST /device {
            '_id': "id",
            'name': 'pretty name', # optional
            'addresses': [ # optional
                {'ip': 'ip address 1', 'port': 1912},
                ...
            ],
        }
            -> {
                'success': True,
                'updated_device_id': "id",
            }

    Delete a device
        DELETE /device/id
            -> {
                'success': True,
                'deleted_device_id': "id",
            }
    """

    def GET(self, id=None):
        self.requireLoggedIn()
        if id is None:
            return self.success({'devices': self.user.get('devices', [])})
        else:
            device = database.devices.find_one({
                '_id': database.ObjectId(id),
                'owner': self.user['_id'],
            })
            device.pop('owner')
            return self.success(device)

    def POST(self):
        self.requireLoggedIn()
        i = self.input
        device = self.data
        if '_id' in device:
            func = self._update
        else:
            func = self._create

        return func(device)

    def _parse_addresses(self, addresses):
        parsed_addresses = []
        for i, addr in enumerate(addresses):
            ip = addr.get('ip', '').strip()
            if not ip:
                raise Exception(
                    "address %d: You have to provide a valid IP" % i
                )
            port = int(addr.get('port', 0))
            if not port:
                raise Exception(
                    "address %d: You have to provide a valid port" % i
                )
            parsed_addresses.append({'ip': ip, 'port': port})
        return parsed_addresses

    def _create(self, device):
        name = device.get('name', '').strip()
        if not name:
            return self.error("You have to provide a valid device name")
        addresses = device.get('addresses', [])
        if not addresses:
            return self.error("You have to provide at least on address")

        try:
            parsed_addresses = self._parse_addresses(addresses)
        except Exception, err:
            return self.error(str(err))

        device = {
            'name': name,
            'addresses': parsed_addresses,
            'owner': self.user['_id'],
        }

        id = database.devices.insert(device)
        assert id is not None

        device['passport'] = metalib.generate_passport(
            id,
            conf.INFINIT_AUTHORITY_PATH,
            conf.INFINIT_AUTHORITY_PASSWORD
        )
        database.devices.save(device)

        # XXX check unique device ?
        self.user.setdefault('devices', []).append(str(id))
        database.users.save(self.user)
        return self.success({
            'created_device_id': str(id),
            'passport': device['passport']
        })

    def _update(self, device):
        assert '_id' in device
        id = device['_id'].strip()
        if not id in self.user['devices']:
            raise web.Forbidden("This network does not belong to you")
        to_save = database.devices.find_one({
            '_id': database.ObjectId(id)
        })
        if 'name' in device:
            name = device['name'].strip()
            if not name:
                return self.error("You have to provide a valid device name")
            to_save['name'] = name
        if 'addresses' in device:
            addresses = device['addresses']
            try:
                addresses = self._parse_addresses(device['addresses'])
            except Exception, err:
                return self.error(str(err))
            to_save['addresses'] = addresses
        id = database.devices.save(to_save)
        return self.success({
            'updated_device_id': str(id),
            'passport': to_save['passport'],
        })

    def DELETE(self, id):
        self.requireLoggedIn()
        try:
            devices = self.user['devices']
            idx = devices.index(id)
            devices.pop(idx)
        except:
            return json.dumps({
                'success': False,
                'error': "The device '%s' was not found" % (id),
            })
        database.users.save(self.user)
        database.devices.find_and_modify({
            '_id': database.ObjectId(id),
            'owner': self.user['_id'], #not required
        }, remove=True)
        return self.success({
            'deleted_device_id': id,
        })

