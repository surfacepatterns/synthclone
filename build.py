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
REVISION = 9

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
    createMenuEntry = False
    createPkgConfig = False
    platformArgs = []
    if mac_ver()[0]:
        defaultPrefix = "/Applications/"
        platform = "MACX"
        platformArgs += ["-spec", "macx-g++"]
    elif win32_ver()[0]:
        defaultPrefix = "C:\\Program Files\\synthclone"
        platform = "WIN32"
    else:
        createMenuEntry = True
        createPkgConfig = True
        defaultPrefix = "/usr/local"
        platform = "UNIX"

    parser = OptionParser("usage: %prog [options] [qmake-args]")

    # Basic options
    parser.add_option("-b", "--build-dir", action="store", default=None,
                      dest="buildDir", help="Build directory")
    parser.add_option("-d", "--debug", action="store_true", default=False,
                      dest="debug", help="Build a debug executable")
    parser.add_option("-m", "--make-dir", action="store", default=None,
                      dest="makeDir", help="Make directory")
    parser.add_option("-p", "--prefix", action="store", default=None,
                      dest="prefix", help="Install prefix")

    # Build options
    parser.add_option("--skip-api-docs", action="store", default=0,
                      dest="skipAPIDocs", help="Don't build API documentation",
                      type="int")
    parser.add_option("--skip-headers", action="store", default=0,
                      dest="skipHeaders", help="Don't build API headers",
                      type="int")
    parser.add_option("--skip-hydrogen", action="store", default=0,
                      dest="skipHydrogen",
                      help="Don't build the Hydrogen plugin", type="int")
    parser.add_option("--skip-jack", action="store", default=0,
                      dest="skipJACK", help="Don't build the JACK plugin",
                      type="int")
    parser.add_option("--skip-sfz", action="store", default=0, dest="skipSFZ",
                      help="Don't build the SFZ plugin", type="int")
    parser.add_option("--skip-trimmer", action="store", default=0,
                      dest="skipTrimmer",
                      help="Don't build the trimmer plugin", type="int")
    parser.add_option("--skip-zone-generator", action="store", default=0,
                      dest="skipZoneGenerator",
                      help="Don't build the zone generator plugin", type="int")

    options, args = parser.parse_args()

    scriptDir = abspath(dirname(argv[0]))
    buildDir = options.buildDir
    if buildDir is None:
        buildDir = join(scriptDir, "build")
    else:
        buildDir = abspath(buildDir)
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
    skipAPIDocs = options.skipAPIDocs
    if not skipAPIDocs:
        docDir = join(scriptDir, "build", "share", "doc", "synthclone-devel")
        if not isdir(docDir):
            makedirs(docDir)
    skipHeaders = options.skipHeaders

    data = {
        "majorVersion": MAJOR_VERSION,
        "minorVersion": MINOR_VERSION,
        "platform": platform,
        "prefix": prefix,
        "revision": REVISION
    }

    # Write templates *before* calling qmake.  In order for qmake to add
    # install items, it must know they exist first.
    if not skipHeaders:
        writeTemplate(join(buildDir, "include", "synthclone", "config.h"),
                      join("templates", "config.h"), data)
    if createMenuEntry:
        writeTemplate(join(buildDir, "share", "applications",
                           "synthclone.desktop"),
                      join("templates", "synthclone.desktop"), data)
    if createPkgConfig:
        writeTemplate(join(buildDir, "lib", "pkgconfig", "synthclone.pc"),
                      join("templates", "synthclone.pc"), data)

    qmakeArgs = ["qmake", "-recursive"] + platformArgs + args + \
        ["MAJOR_VERSION=%d" % MAJOR_VERSION, "MINOR_VERSION=%d" % MINOR_VERSION,
         "REVISION=%d" % REVISION, "BUILDDIR=%s" % buildDir,
         "MAKEDIR=%s" % makeDir, "PREFIX=%s" % prefix]
    if skipAPIDocs:
        qmakeArgs.append("SKIP_API_DOCS=1")
    if skipHeaders:
        qmakeArgs.append("SKIP_HEADERS=1")
    if options.debug:
        qmakeArgs.append("DEBUG=1")
    if options.skipHydrogen:
        qmakeArgs.append("SKIP_HYDROGEN_PLUGIN=1")
    if options.skipJACK:
        qmakeArgs.append("SKIP_JACK_PLUGIN=1")
    if options.skipSFZ:
        qmakeArgs.append("SKIP_SFZ_PLUGIN=1")
    if options.skipTrimmer:
        qmakeArgs.append("SKIP_TRIMMER_PLUGIN=1")
    if options.skipZoneGenerator:
        qmakeArgs.append("SKIP_ZONE_GENERATOR_PLUGIN=1")

    if call(qmakeArgs):
        parser.error("qmake returned an error")
    if call(["make"]):
        parser.error("make returned an error")
    if (not skipAPIDocs) and call(["doxygen", "Doxyfile"]):
        parser.error("documentation generation failed")

    stdout.write("Build successful.  Run `make install` to install.\n")

if __name__ == "__main__":
    main()
