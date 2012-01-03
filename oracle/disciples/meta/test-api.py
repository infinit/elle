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
            raise Exception("Cannot register!")
        res = client.post('/login', {'email': email, 'password': password})
        if not res['success']:
            print res
            raise Exception("Cannot login!")
    session['token'] = res['token']
    print "Got token:", res['token']

    print client.get('/account')
