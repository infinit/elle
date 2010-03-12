import drake, drake.cxx

def configure(cxx_toolkit):

  cfg = drake.cxx.Config()
  cfg.add_local_include_path('./.') # Remove when drake.cxx search in the dir of source files as expected
  sources = drake.nodes('main.cc', 'test.cc')
  executable = drake.cxx.Executable('test', sources, cxx_toolkit, cfg)
