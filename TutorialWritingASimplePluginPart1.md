# Tutorial: Writing a Simple Plugin, Part 1 #

## Overview ##

I'm going to let you on a little secret.  `synthclone`, on its own, can't really do much of anything.  Without a sampler, `synthclone` can't send out MIDI or capture audio.  Without effects, `synthclone` can't modify samples.  Without targets, `synthclone` can't save sample instruments to different formats.  Without tools ... well, you get the idea.  All of these components are provided by plugins.  So, without plugins, `synthclone` is pretty much useless.

Thankfully, writing a plugin for `synthclone` is not very difficult.  In this tutorial, I'm going to teach you how to write a simple plugin for `synthclone`.  Throughout the tutorial, I'll be referencing different classes in the `synthclone` developer API.  With each reference, I'll include a link to the documentation for that class.

In this tutorial, we're going to make a simple plugin that allows the user to create new zones with samples from the hard drive.  This isn't intended to be a complete plugin, but it could very well be the beginning of a more sophisticated plugin.

## What You'll Need ##

You'll need to have the following items installed:

  1. The `synthclone` developer API
  1. `qmake`
  1. An application for editing your code.  I'll be using `emacs`, but you're free to use whatever you'd like in its place.
  1. A working knowledge of C++

## Project File ##

First, you'll need to create a new directory for your project.  Let's call it 'sampleloader':

```
devin@(none):~/Desktop/code-projects$ mkdir sampleloader
devin@(none):~/Desktop/code-projects$ cd sampleloader
devin@(none):~/Desktop/code-projects/sampleloader$
```

`synthclone` plugins are [Qt plugins](http://doc.qt.nokia.com/4.7/plugins-howto.html), compiled using [qmake](http://doc.qt.nokia.com/stable/qmake-manual.html); so, we'll need to create a Qt project file:

```
devin@(none):~/Desktop/code-projects/sampleloader$ emacs sampleloader.pro
```

Initialize the file with the following:

```
CONFIG += plugin link_pkgconfig
DESTDIR = ./lib
PKGCONFIG += synthclone
TARGET = $$qtLibraryTarget(synthclone_sampleloader)
TEMPLATE = lib

INCLUDEPATH += $$system(pkg-config --variable=includedir synthclone)

HEADERS += 
SOURCES += 
```

The first five lines tell `qmake` that you're creating a plugin, where to put the plugin, and that package configuration information from `synthclone` can be used to get the necessary libraries and include directories to compile your project with `libsynthclone`.

The 'INCLUDEPATH' line is a hack to get around [a bug in Qt's meta-object compiler](https://bugreports.qt.nokia.com/browse/QTBUG-22704).  It gets the `synthclone` include directory directly from the pkgconfig data for `synthclone`.

Finally, the 'HEADERS' and 'SOURCES' lines tell `qmake` which files to compile to create your plugin.  Let's create some of those now.

## Plugin Entry Point ##

To start, we're going to need a plugin entry point.  To create a plugin entry point, you'll need to implement an interface in the `synthclone` API named [synthclone::IPlugin](http://wiki.synthclone.googlecode.com/git/doxygen/classsynthclone_1_1IPlugin.html), which is found in 'synthclone/iplugin.h'.

Let's create the plugin entry point in 'plugin.h':

```
devin@(none):~/Desktop/code-projects/sampleloader$ emacs plugin.h
```

Initialize the file with the following:

```
#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include <synthclone/iplugin.h>

#include "participant.h"

class Plugin: public QObject, public synthclone::IPlugin {

    Q_OBJECT
    Q_INTERFACES(synthclone::IPlugin)

public:

    explicit
    Plugin(QObject *parent=0);

    ~Plugin();

    QByteArray
    getId() const;

    synthclone::Participant *
    getParticipant();

private:

    Participant participant;

};

#endif
```

Let's go over some of the header file:

```
#include <synthclone/iplugin.h>

#include "participant.h"
```

The file 'synthclone/iplugin.h' contains the base `synthclone` plugin interface.  The plugin class you're creating is going to implement that interface.

The file 'participant.h' is a file that you're going to create in your project.  It's going to contain the object that interacts with the `synthclone` application.

```
    Q_INTERFACES(synthclone::IPlugin)
```

This line tells Qt that this class implements the 'synthclone::IPlugin' interface.

```
    QByteArray
    getId() const;

    synthclone::Participant *
    getParticipant();
```

Every `synthclone` plugin entry point must implement these methods.  More info on these methods can be found in [the documentation for synthclone::IPlugin](http://wiki.synthclone.googlecode.com/git/doxygen/classsynthclone_1_1IPlugin.html).

```
    Participant participant;
```

You'll be creating the 'Participant' class later in this tutorial.

You'll also need a class definition for your plugin entry point.  Let's create that now:

```
devin@(none):~/Desktop/code-projects/sampleloader$ emacs plugin.cpp
```

Initialize the file with the following:

```
#include "plugin.h"

Plugin::Plugin(QObject *parent):
    QObject(parent)
{
    // Empty
}

Plugin::~Plugin()
{
    // Empty
}

QByteArray
Plugin::getId() const
{
    return "com.example.synthclone.plugins.sampleloader";
}

synthclone::Participant *
Plugin::getParticipant()
{
    return &participant;
}

Q_EXPORT_PLUGIN2(synthclone_sampleloader, Plugin);
```

Let's examine some of the class definition:

```
QByteArray
Plugin::getId() const
{
    return "com.example.synthclone.plugins.sampleloader";
}
```

[QByteArray](http://doc.qt.nokia.com/4.7/qbytearray.html) is a core Qt class that wraps an array of bytes.  `synthclone` plugin ids are discussed in [the documentation for synthclone::IPlugin](http://wiki.synthclone.googlecode.com/git/doxygen/classsynthclone_1_1IPlugin.html).

```
synthclone::Participant *
Plugin::getParticipant()
{
    return &participant;
}
```

This method allows the `synthclone` application to access the 'Participant'.  We'll discuss the 'Participant' shortly.

```
Q_EXPORT_PLUGIN2(synthclone_sampleloader, Plugin);
```

This line lets qmake know that this class is the plugin entry point.

We've created the plugin entry point.  Let's move on to the Participant.

## Plugin Controller (a.k.a. Participant) ##

A [Participant](http://wiki.synthclone.googlecode.com/git/doxygen/classsynthclone_1_1Participant.html) is an object that interacts with the `synthclone` application session.  An activated Participant object can do all sorts of things:

  * Add/Query/Remove/(De)Select/Update zones
  * Add/Remove menu actions and separators
  * Add/Query/Remove components (effects, samplers, targets)
  * Add/Activate/Deactivate/Query/Remove participants
  * Change the session sample rate/channel count
  * Create/Load/Save/Unload sessions
  * Initiate target builds
  * Monitor changes to objects
  * Show custom windows
  * ... and more.

So, how does a Participant do all this?

`synthclone` was created using several design patterns.  In particular, the Participant is able to access the `synthclone` session using the [mediator pattern](http://en.wikipedia.org/wiki/Mediator_pattern) and the [observer pattern](http://en.wikipedia.org/wiki/Observer_pattern), both using a [synthclone::Context](http://wiki.synthclone.googlecode.com/git/doxygen/classsynthclone_1_1Context.html) object, which is passed to a Participant object when it is activated.

We are going to start by creating a minimal Participant.  Let's declare the Participant in 'participant.h':

```
devin@(none):~/Desktop/code-projects/sampleloader$ emacs participant.h
```

Initialize the file with the following:

```
#ifndef __PARTICIPANT_H__
#define __PARTICIPANT_H__

#include <synthclone/participant.h>

class Participant: public synthclone::Participant {

    Q_OBJECT

public:

    explicit
    Participant(QObject *parent=0);

    ~Participant();

    void
    activate(synthclone::Context &context, const QVariant &state=QVariant());

    void
    deactivate(synthclone::Context &context);

private:

    synthclone::Context *context;

};

#endif
```

Let's examine some of the class declaration:

```
    void
    activate(synthclone::Context &context, const QVariant &state=QVariant());

    void
    deactivate(synthclone::Context &context);
```

The 'activate' method is called when the Participant is activated.  The 'context' argument is the mediator that is used to communicate with the `synthclone` application session.  Each Participant has its own unique Context object that is created just before the Participant is activated.  The optional 'state' argument is used to pass state information saved in a previous session.

The 'deactivate' method is called when the Participant is deactivated.  The 'context' argument is the same Context object passed to the 'activate' method.  The Context object will be destroyed shortly after this method returns, so be sure that your Participant stops referencing the Context object when this method is called.

```
    synthclone::Context *context;
```

We're going to use this variable to store the Context object when the Participant is activated.

We'll need to define the methods for the Participant object.  We'll do that in 'participant.cpp':

```
devin@(none):~/Desktop/code-projects/sampleloader$ emacs participant.cpp
```

Initialize the file with the following:

```
#include <QtCore/QDebug>

#include "participant.h"

Participant::Participant(QObject *parent):
    synthclone::Participant(tr("Sample Loader"), 0, 0, 1, "Your Name",
                            tr("Loads samples from the filesystem"), parent)
{
    context = 0;
}

Participant::~Participant()
{
    // Empty
}

void
Participant::activate(synthclone::Context &context, const QVariant &/*state*/)
{
    qDebug() << "Participant activated.";
    this->context = &context;
}

void
Participant::deactivate(synthclone::Context &/*context*/)
{
    qDebug() << "Participant deactivated.";
    this->context = 0;
}
```

Let's examine some of the class definition:

```
#include <QtCore/QDebug>
```

Including this header will allow us to use Qt's debugging facilities.  By default, debug messages will appear on the console.  We'll use 'qDebug()' to output console messages.

```
Participant::Participant(QObject *parent):
    synthclone::Participant(tr("Sample Loader"), 0, 0, 1, "Your Name",
                            tr("Loads samples from the filesystem"), parent)
{
    context = 0;
}
```

Our Participant class calls the constructor with several arguments:

  * The name of the participant
  * The major portion of the version
  * The minor portion of the version
  * The revision portion of the version
  * The author
  * A short summary of what your Participant does

```
void
Participant::activate(synthclone::Context &context, const QVariant &/*state*/)
{
    qDebug() << "Participant activated.";
    this->context = &context;
}
```

When the Participant is activated, it will emit a debug message.

```
void
Participant::deactivate(synthclone::Context &context)
{
    qDebug() << "Participant deactivated.";
    this->context = 0;
}
```

When the Participant is deactivated, it will emit a debug message.

## Back to the Project File ##

We need to make sure we add the source files and header files to the project file.  So, open up the project file:

```
devin@(none):~/Desktop/code-projects/sampleloader$ emacs sampleloader.pro
```

... and change the 'HEADERS' and 'SOURCES' variables to include the header and source files you just created:

```
CONFIG += plugin link_pkgconfig
DESTDIR = ./lib
PKGCONFIG += synthclone
TARGET = $$qtLibraryTarget(synthclone_sampleloader)
TEMPLATE = lib

INCLUDEPATH += $$system(pkg-config --variable=includedir synthclone)

HEADERS += participant.h \
    plugin.h
SOURCES += participant.cpp \
    plugin.cpp
```

It's time to compile your first `synthclone` plugin!

## Compiling the Plugin ##

Compiling a plugin using `qmake` is a two-step process.  First, we run qmake on the project file:

```
devin@(none):~/Desktop/code-projects/sampleloader$ qmake sampleloader.pro
```

`qmake` creates a 'Makefile' for us.  Let's run the Makefile:

```
devin@(none):~/Desktop/code-projects/sampleloader$ make
g++ -c -pipe -O2 -I/usr/include -Wall -W -D_REENTRANT -fPIC -DQT_NO_DEBUG -DQT_PLUGIN -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I/usr/include -I. -o participant.o participant.cpp
g++ -c -pipe -O2 -I/usr/include -Wall -W -D_REENTRANT -fPIC -DQT_NO_DEBUG -DQT_PLUGIN -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I/usr/include -I. -o plugin.o plugin.cpp
/usr/bin/moc-qt4 -DQT_NO_DEBUG -DQT_PLUGIN -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I/usr/include -I. participant.h -o moc_participant.cpp
g++ -c -pipe -O2 -I/usr/include -Wall -W -D_REENTRANT -fPIC -DQT_NO_DEBUG -DQT_PLUGIN -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I/usr/include -I. -o moc_participant.o moc_participant.cpp
/usr/bin/moc-qt4 -DQT_NO_DEBUG -DQT_PLUGIN -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I/usr/include -I. plugin.h -o moc_plugin.cpp
g++ -c -pipe -O2 -I/usr/include -Wall -W -D_REENTRANT -fPIC -DQT_NO_DEBUG -DQT_PLUGIN -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I/usr/include -I. -o moc_plugin.o moc_plugin.cpp
rm -f libsynthclone_sampleloader.so
g++ -Wl,-O1 -shared -o libsynthclone_sampleloader.so participant.o plugin.o moc_participant.o moc_plugin.o   -L/usr/lib -lsynthclone -lQtGui -lQtCore -lpthread  
mv -f libsynthclone_sampleloader.so lib/
```

You should now have a shared library in your directory that contains the plugin.  Let's test the plugin.

## Testing the Plugin ##

First, we need to tell `synthclone` where to find your plugin.  There's an (as of yet) undocumented feature in `synthclone` that allows a user to add directories that `synthclone` should search to find plugins.  In order to test your plugin, we need to add your plugin's directory to that list.  We'll do this by editing the `synthclone` config file in your HOME directory.

(NOTE: If you're never run `synthclone`, the configuration file will not yet be present in your HOME directory.  Make sure you run `synthclone` before you edit the configuration file.)

Let's edit the file now:

```
devin@(none):~/Desktop/code-projects/sampleloader$ emacs ~/.config/synthclone.googlecode.com/synthclone.conf
```

The file should contain a section called `synthclone`, and possibly also contains a list of recent session paths.  Here's an example:

```
[synthclone]
recentSessionPaths=/home/devin/tmp/drumatic-kit, /home/devin/tmp/amsynth-test
```

We want to add a variable named 'pluginPaths' to the 'synthclone' section of the file that contains the directory where you compiled your plugin:

```
[synthclone]
recentSessionPaths=/home/devin/tmp/drumatic-kit, /home/devin/tmp/amsynth-test
pluginPaths=/home/devin/Desktop/code-projects/sampleloader/lib
```

Now, start `synthclone` from the command line:

```
devin@(none):~/Desktop/code-projects/sampleloader$ synthclone
"Translations loaded." 
"Creating core application objects ..." 
"Loading plugins from '/usr/lib/synthclone/plugins' ..." 
"Attempting to load plugin from '/usr/lib/synthclone/plugins/libsynthclone_sfz.so' ..." 
"Participant added to session manager: SFZ 0.0-1" 
"Attempting to load plugin from '/usr/lib/synthclone/plugins/libsynthclone_jack.so' ..." 
"Participant added to session manager: JACK 0.0-1" 
"Attempting to load plugin from '/usr/lib/synthclone/plugins/libsynthclone_hydrogen.so' ..." 
"Participant added to session manager: Hydrogen 0.0-1" 
"Attempting to load plugin from '/usr/lib/synthclone/plugins/libsynthclone_trimmer.so' ..." 
"Participant added to session manager: Trimmer 0.0-1" 
"Attempting to load plugin from '/usr/lib/synthclone/plugins/libsynthclone_zonegenerator.so' ..." 
"Participant added to session manager: Zone Generator 0.0-1" 
"Loading plugins from '/home/devin/Desktop/code-projects/sampleloader/lib' ..." 
"Attempting to load plugin from '/home/devin/Desktop/code-projects/sampleloader/lib/libsynthclone_sampleloader.so' ..." 
"Participant added to session manager: Sample Loader 0.0-1" 
"Core application objects created." 
"Running ..."
```

Right near the end, you can see that `synthclone` loads your participant.  However, your participant has not been activated yet.  To test activation/deactivation of your participant, create an empty session:

![http://wiki.synthclone.googlecode.com/git/images/TutorialWritingASimplePluginPart1-loadsession.png](http://wiki.synthclone.googlecode.com/git/images/TutorialWritingASimplePluginPart1-loadsession.png)

After the new session is initialized, you'll be prompted with the participants dialog.  You can see your participant in the dialog:

![http://wiki.synthclone.googlecode.com/git/images/TutorialWritingASimplePluginPart1-participants.png](http://wiki.synthclone.googlecode.com/git/images/TutorialWritingASimplePluginPart1-participants.png)

Double click the 'Sample Loader' participant to activate it, and then take a look at the console where you started `synthclone`.  You'll notice a couple new lines:

```
Participant activated. 
"Participant activated: Sample Loader 0.0-1"
```

The "Participant activated." line was emit by the activate() method in your participant.  Double click the 'Sample Loader' participant again to deactivate it, and more data will be output on the console:

```
Participant deactivated. 
"Participant deactivated: Sample Loader 0.0-1"
```

As you can see, your participant can be loaded, activated, and deactivated by `synthclone`.

In the [next part of this tutorial](TutorialWritingASimplePluginPart2.md), you'll use more of the functionality of `libsynthclone` to add samples from your hard drive to a `synthclone` session.