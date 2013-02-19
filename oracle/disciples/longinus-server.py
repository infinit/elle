#!/usr/bin/env python2.7
# -*- encoding: utf-8 -*-

"""
longinus-server is an helper for hole punching

Longinus is the name given in medieval and some modern Christian traditions to
the Roman soldier who pierced Jesus in his side with a lance, the "Holy Lance"
(lancea, in the Latin Vulgate) while he was on the Cross.

The figure is unnamed in the gospels.

The Longinus legend further identifies this soldier as the centurion present at
the Crucifixion, who testified, "In truth this man was son of God."

Longinus's legend grew over the years to the point that he was said to have
converted to Christianity after the Crucifixion, and he is traditionally
venerated as a saint in the Catholic Church, Eastern Orthodox Church, and
several other Christian communions.

"""

import sys

from longinus.application import Application

if __name__ == '__main__':
    application = Application("0.0.0.0", 9999)
    application.run()
