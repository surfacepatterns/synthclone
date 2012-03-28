#!/usr/bin/env python

from optparse import OptionParser
from os import listdir, makedirs, sep
from os.path import abspath, basename, dirname, isdir, isfile, join
from platform import mac_ver, win32_ver
from shutil import copyfile
from string import Template
from subprocess import PIPE, Popen, call
from sys import argv, exit, stdout

MAJOR_VERSION = 0
MINOR_VERSION = 1
REVISION = 10

################################################################################
# Mac OSX
################################################################################

def copyLibraries(binaryFile, installPath, libDestinationDir,
                  ignoreDirectories=[], executablePath=None, dependencyMap={}):
    if executablePath is None:
        executablePath = binaryFile
    ignoreDirectories = [(abspath(d) + sep) for d in ignoreDirectories]
    for dependency in getDependencies(binaryFile):
        dependencyPath = join(executablePath, dependency)
        if any((dependencyPath.startswith(d) for d in ignoreDirectories)):
            continue
        libFile = basename(dependency)
        if libFile == basename(binaryFile):
            continue

        # The dependency isn't a self-reference.  See if it's in the dependency
        # map.
        location = dependencyMap.get(libFile)
        if location is None:

            # We'll infer the new location of the dependency.
            destinationPath = join(libDestinationDir, libFile)
            location = join(installPath, libFile)
            if not isfile(destinationPath):

                # The dependency hasn't been copied over.  So, copy it over,
                # and update library references.
                if not isfile(dependencyPath):
                    raise Exception("dependency '%s' does not exist" %
                                    dependencyPath)
                destinationDir = dirname(destinationPath)
                if not isdir(destinationDir):
                    makedirs(destinationDir)
                stdout.write("Copying library '%s' to '%s' ...\n" %
                             (dependencyPath, destinationPath))
                copyfile(dependencyPath, destinationPath)
                copyLibraries(destinationPath, installPath, libDestinationDir,
                              ignoreDirectories, executablePath)
                setLibraryLocation(destinationPath, location)

        replaceDependency(binaryFile, dependency, location)

def getDependencies(binaryFile):
    args = ["otool", "-L", binaryFile]
    process = Popen(args, stdout=PIPE)
    stdoutData, stderrData = process.communicate()
    if process.returncode:
        raise Exception("error getting dependencies for '%s'" % binaryFile)
    dependencies = []
    for s in stdoutData.strip().splitlines()[1:]:
        s = s.strip()
        pos = s.find(" (compatibility ")
        if pos == -1:
            raise Exception("unexpected line output from libtool: '%s'" % s)
        dependencies.append(s[:pos].strip())
    return dependencies

def replaceDependency(binaryFile, find, replace):
    stdout.write("Replacing dependency '%s' with '%s' in '%s' ...\n" %
                 (find, replace, binaryFile))
    args = ["install_name_tool", "-change", find, replace, binaryFile]
    if call(args):
        raise Exception("error replacing dependency '%s' with '%s' in '%s'" %
                        (find, replace, binaryFile))

def setLibraryLocation(libraryFile, location):
    stdout.write("Setting library location of '%s' to '%s' ...\n" %
                 (libraryFile, location))
    args = ["install_name_tool", "-id", location, libraryFile]
    if call(args):
        raise Exception("error setting library location to '%s' for '%s'" %
                        (location, libraryFile))

################################################################################
# Shared code
################################################################################

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
    bundleDynamicLibs = False
    createMenuEntry = False
    createPkgConfig = False
    platformArgs = []
    if mac_ver()[0]:
        bundleDynamicLibs = True
        frameworkDir = join("Library", "Frameworks", "synthclone.framework")
        defaultPrefix = "/"
        libraryDocSuffix = join(frameworkDir, "Documentation")
        platform = "MACX"
        platformArgs += ["-spec", "macx-g++"]
    elif win32_ver()[0]:
        defaultPrefix = "C:\\Program Files\\synthclone"
        libraryDocSuffix = join("share", "doc", "synthclone-devel")
        platform = "WIN32"
    else:
        createMenuEntry = True
        createPkgConfig = True
        defaultPrefix = "/usr/local"
        libraryDocSuffix = join("share", "doc", "synthclone-devel")
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
    parser.add_option("--skip-portmedia", action="store", default=0,
                      dest="skipPortMedia",
                      help="Don't build the PortMedia plugin", type="int")
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
    libraryDocDir = join(scriptDir, "build", libraryDocSuffix)
    skipAPIDocs = options.skipAPIDocs
    if not (skipAPIDocs or isdir(libraryDocDir)):
        makedirs(libraryDocDir)
    skipHeaders = options.skipHeaders

    data = {
        "libraryDocDir": libraryDocDir,
        "majorVersion": MAJOR_VERSION,
        "minorVersion": MINOR_VERSION,
        "platform": platform,
        "prefix": prefix,
        "revision": REVISION
    }

    # Write templates *before* calling qmake.  In order for qmake to add
    # install items, it must know they exist first.
    if not skipAPIDocs:
        writeTemplate(join(makeDir, "Doxyfile"), join("templates", "Doxyfile"),
                      data)
    if not skipHeaders:
        writeTemplate(join(scriptDir, "src", "include", "synthclone",
                           "config.h"),
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
    if options.skipPortMedia:
        qmakeArgs.append("SKIP_PORTMEDIA_PLUGIN=1")
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

    # On Mac, we bundle dynamic libraries in the application and framework
    # bundles.
    if bundleDynamicLibs:
        appDir = join(buildDir, "Applications", "synthclone.app")
        contentsDir = join(appDir, "Contents")
        executablePath = join(contentsDir, "MacOS", "synthclone")
        frameworksDir = join(contentsDir, "Frameworks")
        ignoreDirectories = ["/lib", "/usr/lib", "/Library/Frameworks",
                             "/System/Library/Frameworks",
                             join(prefix, frameworkDir)]
        relativeFrameworksPath = "@executable_path/../Frameworks/"
        version = "%d.%d.%d" % (MAJOR_VERSION, MINOR_VERSION, REVISION)

        # Copy libraries linked to application.
        stdout.write("Copying dependencies for '%s' ...\n" %
                     executablePath)
        if call(["macdeployqt", appDir]):
            parser.error("macdeployqt returned an error")

        # The libraries are now linked correctly in the executable.  Make a map
        # of correctly linked Qt libraries.
        qtDependencyMap = {}
        for dependency in getDependencies(executablePath):
            baseLibName = basename(dependency)
            if baseLibName.startswith("Qt"):
                qtDependencyMap[baseLibName] = dependency

        # Copy libraries linked to plugins.
        pluginsDir = join(contentsDir, "PlugIns")
        for f in listdir(pluginsDir):
            plugin = join(pluginsDir, f)
            if not isfile(plugin):
                continue
            setLibraryLocation(plugin, "@executable_path/../PlugIns/%s" % f)
            stdout.write("Copying dependencies for '%s' ...\n" % plugin)
            copyLibraries(plugin, relativeFrameworksPath, frameworksDir,
                          ignoreDirectories, executablePath, qtDependencyMap)

        # Copy libraries linked to synthclone library.
        libPath = join(buildDir, frameworkDir, "Versions", version,
                       "synthclone")
        stdout.write("Copying dependencies for '%s' ...\n" % libPath)
        copyLibraries(libPath, relativeFrameworksPath, frameworksDir,
                      ignoreDirectories, executablePath, qtDependencyMap)

    # Build documentation
    if (not skipAPIDocs) and call(["doxygen", join(makeDir, "Doxyfile")]):
        parser.error("documentation generation failed")

    stdout.write("Build successful.  Run `make install` to install.\n")

if __name__ == "__main__":
    main()
