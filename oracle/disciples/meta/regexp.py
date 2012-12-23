import re
import meta.error as error

Email = r'^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]{2,20}\.[a-zA-Z]{2,5}$'
Handle = r'^.{3,50}$'
Password = r'^.{64}$'
Device = r'^.{3,50}$'
ID = r'^[a-zA-Z0-9]{24,40}$'
DeviceID = r'^[a-zA-Z0-9]{24,40}$'
NetworkID = r'^[a-zA-Z0-9]{24,40}$'
TransactionID = r'^[a-zA-Z0-9]{24,40}$'
NotNull = r'^.+$'

class Validator:
    def __init__(self, rexp, error_code = error.UNKNOWN):
        self.rexp = re.compile(rexp)
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
