import re
import meta.error as err

Email = r'^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,5}$'
Handle = r'^.{3,50}$'
Password = r'^.{64}$'
Device = r'^.{3,50}$'
ID = r'^[a-zA-Z0-9]{24,40}$'
DeviceID = r'^[a-zA-Z0-9]{24,40}$'
NetworkID = r'^[a-zA-Z0-9]{24,40}$'
NotNull = r'^.+$'

class Validator:
    def __init__(self, rexp, error_code = err.UNKNOWN):
        self.rexp = re.compile(rexp)
        self.error_code = error_code

    def __call__(self, value):
        try:
            result = re.match(self.rexp, value)
            if result:
              result = 0
            else:
              result = error_code
        except:
            result = self.error_code
        return result
