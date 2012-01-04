# -*- encoding: utf-8 -*-

import json
import pymongo.objectid

from meta.page import Page

class Network(Page):
    """
    Ability to
        GET network ids, or one specific network
        POST new one
        DELETE one
    """

    def GET(self, id=None):
        self.requireLoggedIn()
        if id is None:
            return json.dumps(self.user.get('networks', []))
        else:
            return json.dumps(database.networks.find_one({
                '_id': pymongo.objectid.ObjectId(id),
                'owner': self.user['_id'],
            }), default=str)

    def POST(self):
        self.requireLoggedIn()
        i = self.input
        name = i.get('name', '').strip()
        users = map(i.get('users', '').split(','), str.strip)
        devices = map(i.get('devices', '').split(','), str.strip)
        if not name:
            return json.dumps({
                'success': False,
                'error': "You have to provide a valid network name",
            })
        network = {
            'name': name,
            'users': filter(users, lambda u: u != str(self.user['_id'])),
            'devices': filter(devices, lambda d: d in self.user['devices']),
            'owner': self.user['_id'], #usefull ?
        }
        if '_id' in i:
            if i['_id'] not in self.user['networks']:
                raise web.Forbidden("This network does not belong to you")
            network['_id'] = pymongo.objectid.ObjectId(i['_id'])
            id = database.networks.update(network)
        else:
            id = database.networks.insert(network)
        assert id is not None
        self.user.setdefault('networks', []).append(str(id))
        database.users.save(self.user)
        return json.dumps({
            'success': True,
            'network_id': str(id),
        })

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
            'removed_network_id': id,
        })

