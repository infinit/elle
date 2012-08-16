# -*- encoding: utf-8 -*-

"""
Forward imports of metalib
"""
from _metalib import *

#def check_root_block_signature(root_block, root_address, access_block, public_key):
#    """Check wether or not the root block is valid.
#        :param root_block: The base64 encoded root block
#        :param root_address: The base64 encoded root address
#        :param public_key: The base64 encoded public key
#        :rtype: Boolean
#        :return: Wether or not the root_block is valid
#    """
#    return _metalib.check_root_block_signature(root_block, root_address, access_block, public_key)
#
#def generate_identity(id, login, password, authority_file, authority_password):
#    """Generate an identity.
#
#        :param id: The user unique identifier
#        :param login: The user unique id (its email)
#        :param password: The user password
#        :param authority_file: The path to the authority file
#        :param authority_password: Password to decrypt the authority file
#        :rtype: A tuple of two string
#        :return: a pair of strings: the whole identity and the public part of the identity
#    """
#    identity, identity_pub = _metalib.generate_identity(
#        id, login, password, authority_file, authority_password
#    )
#    return (identity, identity_pub)
#
#
#def generate_network_descriptor(network_id, network_name, model, root_address, authority_file, authority_password):
#    """Generate a network descriptor.
#
#        :param network_id: The network unique identifier
#        :param network_id: The network name
#        :param model: Then network model name
#        :param root_address: The base64 encoded root block address
#        :param authority_file: The path to the authority file
#        :param authority_password: Password to decrypt the authority file
#        :rtype: String
#        :return: base64 encoded network descriptor
#    """
#    return _metalib.generate_network_descriptor(
#        network_id, network_name, model, root_address, authority_file, authority_password
#    )
#
#def generate_passport(device_id, authority_file, authority_password):
#    """Generate a passport.
#
#        :param authority_file: The path to the authority file
#        :param authority_password: Password to decrypt the authority file
#        :rtype: String
#        :return: The hex encoded passport
#    """
#    return _metalib.generate_passport(str(device_id), authority_file, authority_password)
#
