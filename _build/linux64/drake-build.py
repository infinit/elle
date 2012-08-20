#!/usr/bin/env python3

import drake
import drake.cxx

cxx_toolkit = drake.cxx.GccToolkit()
cxx_config = drake.cxx.Config()
cxx_config.flag('-fPIC')
cxx_config.enable_debug_symbols()
cxx_config.enable_optimization()

drake.run('../..', cxx_toolkit, cxx_config)
