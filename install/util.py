from os import makedirs, pardir
from os.path import abspath, dirname, isdir, join
from platform import mac_ver, win32_ver
from string import Template

MAJOR_VERSION = 0
MINOR_VERSION = 2
REVISION = 0
VERSION = "%d.%d.%d" % (MAJOR_VERSION, MINOR_VERSION, REVISION)

PLATFORM_MACX = 1
PLATFORM_UNIX = 2
PLATFORM_WIN32 = 3

_PLATFORM = (mac_ver()[0] and PLATFORM_MACX) or \
    (win32_ver()[0] and PLATFORM_WIN32) or PLATFORM_UNIX

def getPlatform():
    return _PLATFORM

def getResourcesDirectory():
    return join(getRootDirectory(), "resources")

def getRootDirectory():
    return abspath(join(dirname(__file__), pardir))

def getTemplatesDirectory():
    return join(getRootDirectory(), "templates")

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
