import logbook
import sys

logbook.StreamHandler(sys.stderr).push_application()
log = logbook.Logger('githooks')
