#
# ---------- imports ----------------------------------------------------------
#

import sys
import web

import resources

#
# ---------- globals ----------------------------------------------------------
#

URLs = (
  '/user/(.+)', 'resources.user.User',
  '/prototype/(.+)', 'resources.prototype.Prototype'
)

application = web.application(URLs, globals(), 12345)

#
# ---------- functions --------------------------------------------------------
#

def     Run(port):
  sys.argv.insert(1, '12345');

  application.run()
