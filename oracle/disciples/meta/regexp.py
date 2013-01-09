import re
import meta.error as error

Email = re.compile(
    r"(^[-!#$%&'*+/=?^_`{}|~0-9A-Z]+(\.[-!#$%&'*+/=?^_`{}|~0-9A-Z]+)*"  # dot-atom
    r'|^"([\001-\010\013\014\016-\037!#-\[\]-\177]|\\[\001-011\013\014\016-\177])*"' # quoted-string
    r')@(?:[A-Z0-9](?:[A-Z0-9-]{0,61}[A-Z0-9])?\.)+[A-Z]{2,6}\.?$', re.IGNORECASE)  # domain

Handle = re.compile(r'^.{3,50}$')
Password = re.compile(r'^.{64}$')
Device = re.compile(r'^.{3,50}$')
ID = re.compile(r'^[a-zA-Z0-9]{24,40}$')
DeviceID = re.compile(r'^[a-zA-Z0-9]{24,40}$')
NetworkID = re.compile(r'^[a-zA-Z0-9]{24,40}$')
TransactionID = re.compile(r'^[a-zA-Z0-9]{24,40}$')
NotNull = re.compile(r'^.+$')

class Validator:
    def __init__(self, rexp, error_code = error.UNKNOWN):
        self.rexp = rexp
        self.error_code = error_code

    def __call__(self, value):
        try:
            result = re.match(self.rexp, value)
            if result:
              result = 0
            else:
              result = self.error_code
        except:
            result = self.error_code
        return result

EmailValidator = Validator(Email, error.EMAIL_NOT_VALID)
PasswordValidator = Validator(Password, error.PASSWORD_NOT_VALID)
HandleValidator = Validator(Handle, error.HANDLE_NOT_VALID)
DeviceIDValidator = Validator(DeviceID, error.DEVICE_NOT_VALID)
NonEmptyValidator = Validator(NotNull, error.FIELD_IS_EMPTY)
FilenameValidator = Validator(NotNull, error.FILE_NAME_EMPTY)
NetworkValidator = Validator(NetworkID, error.NETWORK_ID_NOT_VALID)
TransactionValidator = Validator(TransactionID, error.TRANSACTION_ID_NOT_VALID)
UserIDValidator = Validator(ID, error.USER_ID_NOT_VALID)
