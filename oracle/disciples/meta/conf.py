# -*- encoding: utf-8 -*-
"""
All constants goes here
"""
import os

DEBUG = True
ENCODING = 'utf-8'
SALT = "1nf1n17_S4l7"
SESSION_COOKIE_NAME = 'SESSIONID'
SESSION_TOKEN_NAME = 'token'
SESSION_HEADER_NAME = 'Authorization'

# Name of the mongo collection.
COLLECTION_NAME = os.environ.get("META_COLLECTION_NAME", 'meta')
MONGO_HOST = 'localhost'
MONGO_PORT = 27017

# release install path
INFINIT_AUTHORITY_PATH = os.path.join(
    os.path.abspath(os.path.dirname(__file__)),
    "infinit.auth"
)

if not os.path.exists(INFINIT_AUTHORITY_PATH):
    # When used in build dir
    INFINIT_AUTHORITY_PATH = os.path.join(
        os.path.abspath(os.path.dirname(__file__)),
        "../../../oracle/infinit.auth"
    )

INFINIT_AUTHORITY_PASSWORD = ""

MANDRILL_USERNAME = 'infinitdotio'
MANDRILL_PASSWORD = 'ca159fe5-a0f7-47eb-b9e1-2a8f03b9da86'
MANDRILL_SMTP_HOST = 'smtp.mandrillapp.com'
MANDRILL_SMTP_PORT = 587

LISTEN_TCP_PORT = int(os.environ.get('META_SERVER_PORT', 12345))
TROPHONIUS_LISTEN_SSL_PORT = int(os.environ.get('TROPHONIUS_NOTIFICATION_PORT', 23457))
TROPHONIUS_HOST = "localhost"
