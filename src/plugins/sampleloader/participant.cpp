/*
 * libsynthclone_sampleloader - Sample file loading plugin for `synthclone`
 * Copyright (C) 2011-2012 Devin Anderson
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 * Ave, Cambridge, MA 02139, USA.
 */

#include <synthclone/error.h>

#include "participant.h"

Participant::Participant(QObject *parent):
    synthclone::Participant(tr("Sample Loader"), 0, 0, 2, "Devin Anderson",
                            tr("Loads samples from the filesystem"), parent),
    addSamplesAction(tr("Add Samples"))
{
    context = 0;

    sampleSelectionView.setOperation
        (synthclone::FileSelectionView::OPERATION_OPEN);
    sampleSelectionView.setSelectionFilter
        (synthclone::FileSelectionView::SELECTIONFILTER_EXISTING_FILES);
    sampleSelectionView.setTitle(tr("Add Samples"));

    connect(&addSamplesAction, SIGNAL(triggered()),
            SLOT(handleAddSamplesRequest()));

    connect(&sampleSelectionView, SIGNAL(closeRequest()),
            SLOT(handleCloseRequest()));
    connect(&sampleSelectionView, SIGNAL(pathsSelected(const QStringList &)),
            SLOT(handleSampleSelection(const QStringList &)));
}

Participant::~Participant()
{
    // Empty
}

void
Participant::activate(synthclone::Context &context, const QVariant &/*state*/)
{
    context.addMenuAction(&addSamplesAction, synthclone::MENU_TOOLS);
    this->context = &context;
}

void
Participant::deactivate(synthclone::Context &/*context*/)
{
    context->removeMenuAction(&addSamplesAction);
    this->context = 0;
}

void
Participant::handleAddSamplesRequest()
{
    sampleSelectionView.setVisible(true);
}

void
Participant::handleCloseRequest()
{
    sampleSelectionView.setVisible(false);
}

void
Participant::handleSampleSelection(const QStringList &paths)
{
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
        insertIndex++;

    }

    // Report any errors that occurred during the operation.
    if (errors.count()) {
        context->reportError(errors.join("\n"));
    }

}
