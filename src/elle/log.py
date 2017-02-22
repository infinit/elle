import elle._log
from elle._log import Logger, TextLogger, SysLogger, set_logger

def message(level, type, message):
  import inspect
  frame = inspect.currentframe().f_back.f_back
  file, line, function, source, index = inspect.getframeinfo(frame)
  key = 'ELLE_LOG_COMPONENT'
  if key in frame.f_locals:
    component = frame.f_locals[key]
  else:
    component = frame.f_globals[key]
  return elle._log.Send(level, type, component,
                    file, line, function, message)

def log(msg):
  return message(elle._log.level.log, elle._log.type.info, msg)

def trace(msg):
  return message(elle._log.level.trace, elle._log.type.info, msg)

def debug(msg):
  return message(elle._log.level.debug, elle._log.type.info, msg)

def dump(msg):
  return message(elle._log.level.dump, elle._log.type.info, msg)

def warn(msg):
  return message(elle._log.level.log, elle._log.type.warning, msg)

def err(msg):
  return message(elle._log.level.log, elle._log.type.error, msg)
