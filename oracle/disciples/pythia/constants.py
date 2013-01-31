# -*- encoding: utf-8 -*-
import os

ADMIN_TOKEN = "fdjskfdakl;asdklwqioefwiopfdsjkl;daskl;askl;fsd"
_META_PORT = 'META_SERVER_PORT' in os.environ and os.environ['META_SERVER_PORT'] or 12345
DEFAULT_SERVER = 'http://localhost:%i' % int(_META_PORT)
