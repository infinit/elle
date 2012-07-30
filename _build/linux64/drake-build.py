#!/usr/bin/env python3

import drake
import drake.cxx

cxx_toolkit = drake.cxx.GccToolkit()
cxx_config = drake.cxx.Config()

drake.run('../..', cxx_toolkit, cxx_config)
