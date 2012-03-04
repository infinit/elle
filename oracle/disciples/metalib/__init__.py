# -*- encoding: utf-8 -*-

"""
Forward imports of metalib
"""
import base64

import metalib as _meta
def _b64to16(s):
    return base64.b16encode(base64.b64decode(s, ('+', '-'))).lower()

def check_root_block_signature(root_block, root_address, public_key):
    """Check wether or not the root block is valid.
        :param root_block: The base64 encoded root block
        :param root_address: The base64 encoded root address
        :param public_key: The base64 encoded public key
        :rtype: Boolean
        :return: Wether or not the root_block is valid
    """
    return _meta.check_root_block_signature(root_block, root_address, public_key)

def generate_identity(login, password, authority_file, authority_password):
    """Generate an identity.

        :param login: The user unique id (its email)
        :param password: The user password
        :param authority_file: The path to the authority file
        :param authority_password: Password to decrypt the authority file
        :rtype: A tuple of two string
        :return: a pair of strings: the whole identity and the public part of the identity in hex
    """
    identity, identity_pub = _meta.generate_identity(
        login, password, authority_file, authority_password
    )
    return (_b64to16(identity), _b64to16(identity_pub))


def generate_network_descriptor(network_id, model, root_address, authority_file, authority_password):
    """Generate a network descriptor.

        :param network_id: The network unique identifier
        :param model: Then network model name
        :param root_address: The base64 encoded root block address
        :param authority_file: The path to the authority file
        :param authority_password: Password to decrypt the authority file
        :rtype: String
        :return: base64 encoded network descriptor
    """
    return _meta.generate_network_descriptor(
        network_id, model, root_address, authority_file, authority_password
    )

def generate_passport(authority_file, authority_password):
    """Generate a passport.

        :param authority_file: The path to the authority file
        :param authority_password: Password to decrypt the authority file
        :rtype: String
        :return: The hex encoded passport
    """
    return _b64to16(_meta.generate_passport(authority_file, authority_password))

