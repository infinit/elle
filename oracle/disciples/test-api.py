#!/usr/bin/env python2
# -*- encoding: utf-8 -*-

import json
from datetime import datetime

from api_client import ApiClient

"""
Tests for the sever API
"""

if __name__ == '__main__':
    server = 'http://127.0.0.1:12345'
    session = {}
    client = ApiClient(server, session)

    email = 'testkaka@infinit.io'
    password = 'pifpif'

    res = client.post('/login', {'email': email, 'password': password})
    if not res['success']:
        res = client.post('/register', {'fullname': 'Mister Test', 'email': email, 'password': password})
        if not res['success']:
            raise Exception("Cannot register: " + res['error'])
        res = client.post('/login', {'email': email, 'password': password})
        if not res['success']:
            print res
            raise Exception("Cannot login!")
    session['token'] = res['token']
    print "Got token:", res['token']

    account = client.get('/account')
    print "account:", account
    assert 'success' not in account

    # Testing devices
    if not client.get('/devices'):
        res = client.post('/devices', {'name': "New device", "ip_address": "[::1]"})
        print res
        assert res['success'] is True
        print "added device", res['created_device_id']

    devices = client.get('/devices')
    assert len(devices) > 0
    print "User devices:", devices
    device = client.get('/device/' + devices[0])
    print "Got device", device

    device['name'] = "THIS IS A NEW NAME"
    res = client.post('/devices', device)

    device = client.get('/device/' + devices[0])
    print "Got updated device", device

    assert device['name'] == "THIS IS A NEW NAME"

    res = client.delete('/device/' + devices[0])
    print res
    assert res['success'] is True

    # Testing networks
    if not client.get('/networks'):
        res = client.post('/networks', {'name': "New network"})
        assert res['success'] is True
        print "added network", res['created_network_id']

    networks = client.get('/networks')
    assert len(networks) > 0
    print "user networks", networks

    # updating network
    network = client.get('/network/' + networks[0])
    network['name'] = "THIS IS A NEW `network` NAME"
    res = client.post('/networks', network)
    assert res['success'] is True

    network = client.get('/network/' + networks[0])
    assert network['name'] == "THIS IS A NEW `network` NAME"
    print "updated network", network

    res = client.post('/register', {'fullname': 'Mister Test friend', 'email': "friend@test.net", 'password': password})

    assert client.get('/logout')['success'] is True

