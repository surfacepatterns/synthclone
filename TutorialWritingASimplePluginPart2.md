# Tutorial: Writing a Simple Plugin, Part 2 #

## Overview ##

In the [first part of this tutorial](TutorialWritingASimplePluginPart1.md), you created a Qt project to build a `synthclone` plugin.  You created a plugin entry point, and a basic participant that emitted debug messages when it is activated or eactivated.

In this part of the tutorial, you're going to write code to interact with the `synthclone` session.  You'll add a menu action to the `synthclone` application interface, and create a file selection view that is shown when the menu action is triggered.  The user will be able to use the file selection view to select samples to add to the session.  You'll add those samples to the session by creating a new zone for each sample, and setting each zone's 'Dry Sample' property to contain the contents of the file(s) the user has selected.

Let's get started.

## Declarations ##

You're going to need to add some new members and methods to your Participant class.  Open up the file containing the Participant class declaration:

```
devin@(none):~/Desktop/code-projects/sampleloader$ emacs participant.h
```

Add the following variables to the 'private' section of your class:

```
    synthclone::MenuAction addSamplesAction;
    synthclone::FileSelectionView sampleSelectionView;
```

The menu action will be added to the `synthclone` interface.  In the class definition, we'll add a Qt slot that allows us to know when the menu action is triggered.  When the menu action is triggered, you'll show the file selection view that will allow the user to choose which samples to load into the session.

[synthclone::MenuAction](http://wiki.synthclone.googlecode.com/git/doxygen/classsynthclone_1_1MenuAction.html) and [synthclone::FileSelectionView](http://wiki.synthclone.googlecode.com/git/doxygen/classsynthclone_1_1FileSelectionView.html) are both included in the `synthclone` API in 'synthclone/menuaction.h' and 'synthclone/fileselectionview.h' respectively.

You're going to need to create a few Qt slots to handle signals emitted by the new objects.  Just before the 'private' section, add the following:

```
private slots:

    void
    handleAddSamplesRequest();

    void
    handleSampleSelectionViewCloseRequest();

    void
    handleSampleSelectionViewSelection(const QStringList &paths);
```

`handleAddSamplesRequest()` will be used to capture the signal sent by the menu action we'll add to the interface.  `handleSampleSelectionViewCloseRequest()` will capture the signal from the sample selection view that tells us that the close button has been pressed.  `handleSampleSelectionViewSelection()` will capture the signal from the sample selection view that tells us that the user has selected one or more files to add.

Finally, you'll need to add a directive to include 'synthclone/fileselectionview.h' in your header file:

```
#include <synthclone/fileselectionview.h>
```

The final header file should look something like this:

```
#ifndef __PARTICIPANT_H__
#define __PARTICIPANT_H__

#include <synthclone/fileselectionview.h>
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

private slots:

    void
    handleAddSamplesRequest();

    void
    handleSampleSelectionViewCloseRequest();

    void
    handleSampleSelectionViewSelection(const QStringList &paths);

private:

    synthclone::MenuAction addSamplesAction;
    synthclone::Context *context;
    synthclone::FileSelectionView sampleSelectionView;

};

#endif
```

Now, it's time to write the methods and connect the necessary signals and slots together.

## Initialization ##

Open up 'participant.cpp' in an editor:

```
devin@(none):~/Desktop/code-projects/sampleloader$ emacs participant.cpp
```

You're going to start by modifying the constructor:

```
Participant::Participant(QObject *parent):
    synthclone::Participant(tr("Sample Loader"), 0, 0, 1, "Your Name",
                            tr("Loads samples from the filesystem"), parent),
    addSamplesAction(tr("Add Samples"))
{
    context = 0;

    // Setup the sample selection view
    sampleSelectionView.setOperation
        (synthclone::FileSelectionView::OPERATION_OPEN);
    sampleSelectionView.setSelectionFilter
        (synthclone::FileSelectionView::SELECTIONFILTER_EXISTING_FILES);
    sampleSelectionView.setTitle(tr("Add Samples"));

    // Connect menu action signal to participant slot
    connect(&addSamplesAction, SIGNAL(triggered()),
            SLOT(handleAddSamplesRequest()));

    // Connect sample selection view signals to participant slots
    connect(&sampleSelectionView, SIGNAL(closeRequest()),
            SLOT(handleSampleSelectionViewCloseRequest()));
    connect(&sampleSelectionView, SIGNAL(pathsSelected(const QStringList &)),
            SLOT(handleSampleSelectionViewSelection(const QStringList &)));
}
```

Let's look at what you're adding here:

```
    addSamplesAction(tr("Add Samples"))
```

This calls the constructor for the menu action.  It initializes the text that's displayed by the menu action to "Add Samples".

```
    sampleSelectionView.setOperation
        (synthclone::FileSelectionView::OPERATION_OPEN);
    sampleSelectionView.setSelectionFilter
        (synthclone::FileSelectionView::SELECTIONFILTER_EXISTING_FILES);
    sampleSelectionView.setTitle(tr("Add Samples"));
```

Here, you're setting some basic options for the [synthclone::FileSelectionView](http://wiki.synthclone.googlecode.com/git/doxygen/classsynthclone_1_1FileSelectionView.html) object.

```
    connect(&addSamplesAction, SIGNAL(triggered()),
            SLOT(handleAddSamplesRequest()));

    connect(&sampleSelectionView, SIGNAL(closeRequest()),
            SLOT(handleSampleSelectionViewCloseRequest()));
    connect(&sampleSelectionView, SIGNAL(pathsSelected(const QStringList &)),
            SLOT(handleSampleSelectionViewSelection(const QStringList &)));
```

This is how you connect signals to slots in Qt.  If you've never used Qt signals and slots, this would be a good time to read [the documentation on Qt signals and slots](http://doc.qt.nokia.com/stable/signalsandslots.html).

## Adding/Removing Menu Actions ##

You've created the menu action, but, in order for it to be useful, you need to add it to the application when the participant is activated, and remove it from the application when the participant is deactivated.  Do this in the `activate()` and `deactivate()` methods:

```
void
Participant::activate(synthclone::Context &context, const QVariant &/*state*/)
{
    qDebug() << "Participant activated.";
    context.addMenuAction(&addSamplesAction, synthclone::MENU_TOOLS);
    this->context = &context;
}

void
Participant::deactivate(synthclone::Context &/*context*/)
{
    qDebug() << "Participant deactivated.";
    context->removeMenuAction(&addSamplesAction);
    this->context = 0;
}
```

In this case, we're going to add the action to the 'Tools' menu, but you can add actions to [any menu](http://wiki.synthclone.googlecode.com/git/doxygen/types_8h.html) (scroll down to the Menu enum)  in `synthclone`.

## Handling signals ##

Start by writing a signal handler to handle the case when your [synthclone::MenuAction](http://wiki.synthclone.googlecode.com/git/doxygen/classsynthclone_1_1MenuAction.html) is triggered:

```
void
Participant::handleAddSamplesRequest()
{
    sampleSelectionView.setVisible(true);
}
```

Simple, right?

Handling the close request from your [synthclone::FileSelectionView](http://wiki.synthclone.googlecode.com/git/doxygen/classsynthclone_1_1FileSelectionView.html) object is just as simple:

```
void
Participant::handleSampleSelectionViewCloseRequest()
{
    sampleSelectionView.setVisible(false);
}
```

Now, it's time to write the slot that handles file selection.  This is a little more complex:

```
void
Participant::handleSampleSelectionViewSelection(const QStringList &paths)
{
    // Close the sample selection view.
    sampleSelectionView.setVisible(false);

    int count = paths.count();
    QStringList errors;

    // Insert new zones at the index of the first selected zone, or at the end
    // if no zones are selected.
    int insertIndex = context->getSelectedZoneCount() ?
        context->getZoneIndex(context->getSelectedZone(0)) :
        context->getZoneCount();

    for (int i = 0; i < count; i++) {
        QString path = paths[i];
        try {

            // Make sure the object at the given path is a valid sample file.
            // If the object isn't valid, then one of these constructors will
            // raise a `synthclone::Error`.
            synthclone::Sample sample(path);
            synthclone::SampleInputStream stream(sample);

            // Check the total time consumed by the sample.  If it isn't in the
            // acceptable range for `synthclone` dry samples, then raise a
            // `synthclone::Error`.
            synthclone::SampleRate sampleRate = stream.getSampleRate();
            synthclone::SampleTime time = stream.getFrames() /
                static_cast<synthclone::SampleTime>(sampleRate);
            QString message;
            if (time > synthclone::SAMPLE_TIME_MAXIMUM) {
                message = tr("%1: sample time is %2, which is greater than %3 "
                             "seconds").
                    arg(path).arg(time).arg(synthclone::SAMPLE_TIME_MAXIMUM);
                throw synthclone::Error(message);
            }
            if (time < synthclone::SAMPLE_TIME_MINIMUM) {
                message = tr("%1: sample time is %2, which is less than %3 "
                             "seconds").
                    arg(path).arg(time).arg(synthclone::SAMPLE_TIME_MINIMUM);
                throw synthclone::Error(message);
            }

            // Create a new zone for this sample, and set the sample time and
            // dry sample properties approriately.  Make sure to remove the
            // zone if an exception is thrown, and propagate the exception to
            // the outer catch.
            synthclone::Zone *zone = context->addZone(insertIndex);
            try {
                zone->setSampleTime(time);
                zone->setDrySample(&sample);
            } catch (...) {
                context->removeZone(zone);
                throw;
            }

        } catch (synthclone::Error &e) {

            // Record errors that occur.
            errors.append(e.getMessage());

            continue;
        }

        // We've successfully added a zone with a sample from the hard drive.
        qDebug() << "Added zone with sample at path " << path;
        insertIndex++;

    }

    // Report any errors that occurred during the operation.
    if (errors.count()) {
        context->reportError(errors.join("\n"));
    }
}
```

Follow the comments to understand what this method is doing.

You're using a few new `synthclone` classes:

  * [synthclone::Error](http://wiki.synthclone.googlecode.com/git/doxygen/classsynthclone_1_1Error.html): An exception class used by `synthclone` when an exception is thrown
  * [synthclone::Sample](http://wiki.synthclone.googlecode.com/git/doxygen/classsynthclone_1_1Sample.html): Serves as a simple wrapper around a file object
  * [synthclone::SampleInputStream](http://wiki.synthclone.googlecode.com/git/doxygen/classsynthclone_1_1SampleInputStream.html): Allows you to get information about a sample, and read data sample data

... and a couple new types, which are both defined in [synthclone/types.h](http://wiki.synthclone.googlecode.com/git/doxygen/types_8h.html):

  * synthclone::SampleRate
  * synthclone::SampleTime

In order to use `synthclone::Error`, you'll need to include the header file that contains the declaration at the top of the class:

```
#include <synthclone/error.h>
```

Your final Participant class should look something like this:

```
#include <QtCore/QDebug>

#include <synthclone/error.h>

#include "participant.h"

Participant::Participant(QObject *parent):
    synthclone::Participant(tr("Sample Loader"), 0, 0, 1, "Your Name",
                            tr("Loads samples from the filesystem"), parent),
    addSamplesAction(tr("Add Samples"))
{
    context = 0;

    // Setup the sample selection view
    sampleSelectionView.setOperation
        (synthclone::FileSelectionView::OPERATION_OPEN);
    sampleSelectionView.setSelectionFilter
        (synthclone::FileSelectionView::SELECTIONFILTER_EXISTING_FILES);
    sampleSelectionView.setTitle(tr("Add Samples"));

    // Connect menu action signal to participant slot
    connect(&addSamplesAction, SIGNAL(triggered()),
            SLOT(handleAddSamplesRequest()));

    // Connect sample selection view signals to participant slots
    connect(&sampleSelectionView, SIGNAL(closeRequest()),
            SLOT(handleSampleSelectionViewCloseRequest()));
    connect(&sampleSelectionView, SIGNAL(pathsSelected(const QStringList &)),
            SLOT(handleSampleSelectionViewSelection(const QStringList &)));
}

Participant::~Participant()
{
    // Empty
}

void
Participant::activate(synthclone::Context &context, const QVariant &/*state*/)
{
    qDebug() << "Participant activated.";
    context.addMenuAction(&addSamplesAction, synthclone::MENU_TOOLS);
    this->context = &context;
}

void
Participant::deactivate(synthclone::Context &/*context*/)
{
    qDebug() << "Participant deactivated.";
    context->removeMenuAction(&addSamplesAction);
    this->context = 0;
}

void
Participant::handleAddSamplesRequest()
{
    sampleSelectionView.setVisible(true);
}

void
Participant::handleSampleSelectionViewCloseRequest()
{
    sampleSelectionView.setVisible(false);
}

void
Participant::handleSampleSelectionViewSelection(const QStringList &paths)
{
    // Close the sample selection view.
    sampleSelectionView.setVisible(false);

    int count = paths.count();
    QStringList errors;

    // Insert new zones at the index of the first selected zone, or at the end
    // if no zones are selected.
    int insertIndex = context->getSelectedZoneCount() ?
        context->getZoneIndex(context->getSelectedZone(0)) :
        context->getZoneCount();

    for (int i = 0; i < count; i++) {
        QString path = paths[i];
        try {

            // Make sure the object at the given path is a valid sample file.
            // If the object isn't valid, then one of these constructors will
            // raise a `synthclone::Error`.
            synthclone::Sample sample(path);
            synthclone::SampleInputStream stream(sample);

            // Check the total time consumed by the sample.  If it isn't in the
            // acceptable range for `synthclone` dry samples, then raise a
            // `synthclone::Error`.
            synthclone::SampleRate sampleRate = stream.getSampleRate();
            synthclone::SampleTime time = stream.getFrames() /
                static_cast<synthclone::SampleTime>(sampleRate);
            QString message;
            if (time > synthclone::SAMPLE_TIME_MAXIMUM) {
                message = tr("%1: sample time is %2, which is greater than %3 "
                             "seconds").
                    arg(path).arg(time).arg(synthclone::SAMPLE_TIME_MAXIMUM);
                throw synthclone::Error(message);
            }
            if (time < synthclone::SAMPLE_TIME_MINIMUM) {
                message = tr("%1: sample time is %2, which is less than %3 "
                             "seconds").
                    arg(path).arg(time).arg(synthclone::SAMPLE_TIME_MINIMUM);
                throw synthclone::Error(message);
            }

            // Create a new zone for this sample, and set the sample time and
            // dry sample properties approriately.  Make sure to remove the
            // zone if an exception is thrown, and propagate the exception to
            // the outer catch.
            synthclone::Zone *zone = context->addZone(insertIndex);
            try {
                zone->setSampleTime(time);
                zone->setDrySample(&sample);
            } catch (...) {
                context->removeZone(zone);
                throw;
            }

        } catch (synthclone::Error &e) {

            // Record errors that occur.
            errors.append(e.getMessage());

            continue;
        }

        // We've successfully added a zone with a sample from the hard drive.
        qDebug() << "Added zone with sample at path " << path;
        insertIndex++;

    }

    // Report any errors that occurred during the operation.
    if (errors.count()) {
        context->reportError(errors.join("\n"));
    }
}
```

## Compile and Test ##

As before, go ahead and run `qmake` and `make` to recompile the plugin:

```
devin@(none):~/Desktop/code-projects/sampleloader$ qmake
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

Start `synthclone` at the command line, load a new session, and activate your plugin.  Now, go ahead and click on the 'Tools' menu.  You'll notice a new menu entry:

![http://wiki.synthclone.googlecode.com/git/images/TutorialWritingASimplePluginPart2-toolsmenu.png](http://wiki.synthclone.googlecode.com/git/images/TutorialWritingASimplePluginPart2-toolsmenu.png)

Select the 'Add Samples' menu action, and you'll see a dialog box pop up that allows you to select files:

![http://wiki.synthclone.googlecode.com/git/images/TutorialWritingASimplePluginPart2-addsamplesdialog.png](http://wiki.synthclone.googlecode.com/git/images/TutorialWritingASimplePluginPart2-addsamplesdialog.png)

Find some samples on your hard drive and select those samples:

![http://wiki.synthclone.googlecode.com/git/images/TutorialWritingASimplePluginPart2-samplesselected.png](http://wiki.synthclone.googlecode.com/git/images/TutorialWritingASimplePluginPart2-samplesselected.png)

Open the samples.  You should see some data printed to the console:

```
Added zone with sample at path  "/home/devin/Desktop/noisy-kit/channel1-note35-velocity127-1.ogg" 
Added zone with sample at path  "/home/devin/Desktop/noisy-kit/channel1-note37-velocity127-1.ogg" 
Added zone with sample at path  "/home/devin/Desktop/noisy-kit/channel1-note38-velocity127-1.ogg" 
Added zone with sample at path  "/home/devin/Desktop/noisy-kit/channel1-note39-velocity127-1.ogg" 
Added zone with sample at path  "/home/devin/Desktop/noisy-kit/channel1-note41-velocity127-1.ogg" 
Added zone with sample at path  "/home/devin/Desktop/noisy-kit/channel1-note42-velocity127-1.ogg" 
Added zone with sample at path  "/home/devin/Desktop/noisy-kit/channel1-note44-velocity127-1.ogg" 
Added zone with sample at path  "/home/devin/Desktop/noisy-kit/channel1-note46-velocity127-1.ogg" 
Added zone with sample at path  "/home/devin/Desktop/noisy-kit/channel1-note47-velocity127-1.ogg"
```

... and some zones appear in the `synthclone` interface containing the samples you selected:

![http://wiki.synthclone.googlecode.com/git/images/TutorialWritingASimplePluginPart2-samplesloaded.png](http://wiki.synthclone.googlecode.com/git/images/TutorialWritingASimplePluginPart2-samplesloaded.png)

## What's Next? ##

You've built your first useful `synthclone` plugin.  However, there's much more you can do with the `synthclone` API.  Using the knowledge that you've gained through this tutorial, you can more easily explore and understand the [API documentation](http://wiki.synthclone.googlecode.com/git/doxygen/index.html), and learn how to create more powerful plugins for `synthclone`.  I urge you to look at the [source code for synthclone and the plugins distributed with synthclone](http://code.google.com/p/synthclone/source/browse/), which may give you ideas about plugins you can write and contribute to `synthclone`.

Thank you for taking the time to read and understand this tutorial.  If you have any questions about `synthclone` development, join the [synthclone-development group](http://groups.google.com/group/synthclone-development), and I'll be happy to answer your questions.