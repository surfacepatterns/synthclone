#!/usr/bin/env python

from optparse import OptionParser
from os import makedirs
from os.path import abspath, dirname, isdir, join
from platform import mac_ver, win32_ver
from string import Template
from subprocess import call
from sys import argv, exit, stdout

MAJOR_VERSION = 0
MINOR_VERSION = 1
REVISION = 5

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

def main():
    parser = OptionParser("usage: %prog [options] [qmake-args]")
    parser.add_option("-b", "--build-dir", action="store", default=None,
                      dest="buildDir", help="Build directory")
    parser.add_option("-d", "--debug", action="store_true", default=False,
                      dest="debug", help="Build a debug executable")
    parser.add_option("-m", "--make-dir", action="store", default=None,
                      dest="makeDir", help="Make directory")
    parser.add_option("-p", "--prefix", action="store", default=None,
                      dest="prefix", help="Install prefix")
    options, args = parser.parse_args()

    if mac_ver()[0]:
        defaultPrefix = "/Applications/"
        platform = "MACX"
    elif win32_ver()[0]:
        defaultPrefix = "C:\\Program Files\\synthclone"
        platform = "WIN32"
    else:
        defaultPrefix = "/usr/local"
        platform = "UNIX"

    scriptDir = abspath(dirname(argv[0]))
    buildDir = options.buildDir
    if buildDir is None:
        buildDir = join(scriptDir, "build")
    else:
        buildDir = abspath(buildDir)
    debug = options.debug
    makeDir = options.makeDir
    if makeDir is None:
        makeDir = join(scriptDir, "make")
    else:
        makeDir = abspath(makeDir)
    prefix = options.prefix
    if prefix is None:
        prefix = defaultPrefix
    else:
        prefix = abspath(prefix)
    docDir = join(scriptDir, "build", "share", "doc", "synthclone")
    if not isdir(docDir):
        makedirs(docDir)

    data = {
        "majorVersion": MAJOR_VERSION,
        "minorVersion": MINOR_VERSION,
        "platform": platform,
        "prefix": prefix,
        "revision": REVISION
    }

    # Write templates *before* calling qmake.  In order for qmake to add
    # install items, it must know they exist first.
    writeTemplate(join(buildDir, "include", "synthclone", "config.h"),
                  join("templates", "config.h"), data)
    writeTemplate(join(buildDir, "share", "applications", "synthclone.desktop"),
                  join("templates", "synthclone.desktop"), data)

    qmakeArgs = ["qmake", "-recursive"] + args + \
        ["MAJOR_VERSION=%d" % MAJOR_VERSION, "MINOR_VERSION=%d" % MINOR_VERSION,
         "REVISION=%d" % REVISION, "BUILDDIR=%s" % buildDir,
         "MAKEDIR=%s" % makeDir, "PREFIX=%s" % prefix]
    if debug:
        qmakeArgs.append("DEBUG=1")

    if call(qmakeArgs):
        parser.error("qmake returned an error")
    if call(["make"]):
        parser.error("make returned an error")
    if call(["doxygen", "Doxyfile"]):
        parser.error("documentation generation failed")

    stdout.write("Build successful.  Run `make install` to install.\n")

if __name__ == "__main__":
    main()
