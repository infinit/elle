## -*- coding: utf-8 -*-
import web
import hashlib
SALTY_GOODNESS = u'5gR3dvtres$@;1£56u&&$"FDergjngrekjkj4223234erwgnkvgdvdfkm'

#Must be set by the thing importing this module
collection = None
session = None

def get_user():
    try:
        u = collection.find_one({'_id':session._user_id}) 
        return u
    except AttributeError:
        return None

def authenticate(username, password):
    user = collection.find_one({'username':username, 'password':pswd(password)})
    return user if user else None

def login(user):
    session._user_id = user['_id']
    return user

def logout():
    session.kill()

def register(**kwargs):
    user = collection.save(kwargs)
    return user
    
def pswd(password):
    seasoned = password + SALTY_GOODNESS
    seasoned = seasoned.encode('utf-8')
    return hashlib.md5(seasoned).hexdigest()

def login_required(function, login_page='/login/'):
    def inner(*args, **kwargs):
        if get_user():
            return function(*args, **kwargs)
        else:
            return web.seeother(login_page+'?next=%s' % web.ctx.get('path','/'))
    return inner
