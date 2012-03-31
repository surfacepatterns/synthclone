from os import makedirs, pardir
from os.path import abspath, dirname, isdir, join
from platform import mac_ver, win32_ver
from string import Template

MAJOR_VERSION = 0
MINOR_VERSION = 1
REVISION = 10

PLATFORM_MACX = 1
PLATFORM_UNIX = 2
PLATFORM_WIN32 = 3

_PLATFORM = (mac_ver()[0] and PLATFORM_MACX) or \
    (win32_ver()[0] and PLATFORM_WIN32) or PLATFORM_UNIX

def getPlatform():
    return _PLATFORM

def getResourcesDirectory():
    return abspath(join(dirname(__file__), pardir, "resources"))

def getTemplatesDirectory():
    return abspath(join(dirname(__file__), pardir, "templates"))

def writeTemplate(destination, source, data):
    destination = abspath(destination)
    destinationDir = dirname(destination)
    if not isdir(destinationDir):
        makedirs(destinationDir)
    source = abspath(source)
    sourceDir = dirname(source)
    if not isdir(sourceDir):
        makedirs(sourceDir)
    fp = file(source)
    try:
        s = fp.read()
    finally:
        fp.close()
    s = Template(s).substitute(data)
    fp = file(destination, 'w')
    try:
        fp.write(s)
    finally:
        fp.close()
