# -*- encoding: utf-8 -*-
"""
All constants goes here
"""
import os

DEBUG = True
ENCODING = 'utf-8'
SALT = u"1nf1n17_S4l7"
SESSION_COOKIE_NAME = 'SESSIONID'
SESSION_TOKEN_NAME = 'token'
SESSION_HEADER_NAME = 'Authorization'

INFINIT_AUTHORITY_PATH = os.path.join(
    os.path.abspath(os.path.dirname(__file__)),
    "../../infinit.auth"
)

INFINIT_AUTHORITY_PASSWORD = ""

MANDRILL_USERNAME = 'infinitdotio'
MANDRILL_PASSWORD = 'ca159fe5-a0f7-47eb-b9e1-2a8f03b9da86'
MANDRILL_SMTP_HOST = 'smtp.mandrillapp.com'
MANDRILL_SMTP_PORT = 587
