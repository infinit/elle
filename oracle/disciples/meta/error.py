# Generic.
BAD_REQUEST = 1
ALREADY_LOGGED_IN = 10
NOT_LOGGED_IN = 11
EMAIL_NOT_VALID = 500
HANDLE_NOT_VALID = 501
DEVICE_NOT_VALID = 502
PASSWORD_NOT_VALID = 503
USER_ID_NOT_VALID = 504
NETWORK_ID_NOT_VALID = 505
DEVICE_ID_NOT_VALID = 506
FIELD_IS_EMPTY = 507
ACTIVATION_CODE_NOT_VALID = 508

DEPRECATED = 888

#############
## USER
# Register.
EMAIL_ALREADY_REGISTRED = 10003
HANDLE_ALREADY_REGISTRED = 10005
DEVICE_ALREADY_REGISTRED = 10008
ACTIVATION_CODE_DOESNT_EXIST = 10009

# Login.
EMAIL_PASSWORD_DONT_MATCH = 10101

# Search.
UNKNOWN_USER = 10201

# Swaggers.


############
## Network.
USER_ALREADY_IN_NETWORK = 20001
NETWORK_NOT_FOUND = 20002
DEVICE_NOT_FOUND = 20003
DEVICE_NOT_IN_NETWORK = 20004

ROOT_BLOCK_ALREADY_EXIST = 20005
ROOT_BLOCK_BADLY_SIGNED = 20006


#############
## Device.
USER_ALREADY_INVITED = 30001
USER_ALREADY_IN_INFINIT = 30002

FILE_NAME_EMPTY = 40000

# Other.
UNKNOWN = 666666



error_details = {
    BAD_REQUEST: "This request is bad formed.",
    ALREADY_LOGGED_IN: "You are already logged in.",
    NOT_LOGGED_IN: "You must be logged in.",
    EMAIL_NOT_VALID: "This email is not valid.",
    HANDLE_NOT_VALID: "This handle is not valid, must be 3 to 50 long.",
    PASSWORD_NOT_VALID: "This password is not valid.", #pswd hashed client side.
    NETWORK_ID_NOT_VALID: "This network id is not valid.",
    DEVICE_NOT_VALID: "This device name is not valid.",
    DEVICE_ID_NOT_VALID: "This device id is not valid.",
    ACTIVATION_CODE_NOT_VALID: "This activation code is not valid.",
    FIELD_IS_EMPTY: "This field is empty.",
    DEPRECATED: "Some args are deprecated.",

    # Register.
    EMAIL_ALREADY_REGISTRED: "This email has already been taken.",
    HANDLE_ALREADY_REGISTRED: "This handle has already been taken.",
    DEVICE_ALREADY_REGISTRED: "This device name has already been taken.",
    ACTIVATION_CODE_DOESNT_EXIST: "This activation code doesn't match any.",

    # Login.
    EMAIL_PASSWORD_DONT_MATCH: "Login/Password don't match.",

    # Search.
    UNKNOWN_USER: "This user doesn't seems to exist.",

    # Network.
    USER_ALREADY_IN_NETWORK: "This user is already in the network.",
    NETWORK_NOT_FOUND: "Network not found.",
    DEVICE_NOT_FOUND: "Device not found.",
    DEVICE_NOT_IN_NETWORK: "Cannot find the device in this network.",
    ROOT_BLOCK_ALREADY_EXIST: "This network has already a root block.",
    ROOT_BLOCK_BADLY_SIGNED: "The root block was not properly signed",

    FILE_NAME_EMPTY: "The name is null.",

    USER_ALREADY_INVITED: "You already invited this user.",
    USER_ALREADY_IN_INFINIT: "This user already use infinit.",

    # Other.
    UNKNOWN: "Unknown error.",
}
