# -*- encoding: utf-8 -*-
#
# ---------- imports ----------------------------------------------------------
#

import web
import json

from meta import database

#
# ---------- classes ----------------------------------------------------------
#

class Prototype:
  def GET(self, name):
    devices = []

    for device in database.devices.find({'name': name}):
      devices += [ device['locus'] ]
    return json.dumps(devices)

  def POST(self, name):
    return json.dumps(None)

  def PUT(self, name):
    data = json.loads(web.data())

    for locus in data["loci"]:
      if database.devices.find_one({'name': name, 'locus': locus}):
          continue
      database.devices.insert({'name': name, 'locus': locus})
    return json.dumps(None)

  def DELETE(self, name):
    return json.dumps(None)
