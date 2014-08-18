#!/usr/bin/env python

import logging as log
import sys

log.basicConfig(format = "%(filename)s:%(lineno)d: %(levelname)s: %(message)s",
                level = log.DEBUG,
                stream = sys.stderr)

log.addLevelName(log.CRITICAL,  "critical")
log.addLevelName("critical", log.CRITICAL)

log.addLevelName(log.ERROR,  "error")
log.addLevelName("error", log.ERROR)

log.addLevelName(log.INFO,  "info")
log.addLevelName("info", log.INFO)

log.addLevelName(log.WARNING,  "warning")
log.addLevelName("warning", log.WARNING)

log.addLevelName(log.DEBUG,  "debug")
log.addLevelName("debug", log.DEBUG)

def test():
    "Test setup_logging"
    log.info("An info message")
    log.debug("A debug message")
    log.warning("A warning message")
    log.error("An error message")
    log.critical("A critical message")

if __name__ == '__main__':
    test()
