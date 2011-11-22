#
# ---------- imports ----------------------------------------------------------
#

import web

import database

#
# ---------- globals ----------------------------------------------------------
#

templetor = web.template.render('templates/')

#
# ---------- indexes ----------------------------------------------------------
#

database.users.ensure_index('name', database.ASCENDING,
                            kwags = { 'unique': True})
database.users.ensure_index('email', database.ASCENDING,
                            kwags = { 'unique': True})

#
# ---------- classes ----------------------------------------------------------
#

class User:
  def GET(self, name):
    user = database.users.find_one({'name': name})

    return templetor.user(user)

  def DELETE(self, name):
    print "DELETE " + name

    return "suce<avale>"
