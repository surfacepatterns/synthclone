/*
 * libsynthclone - a plugin API for `synthclone`
 * Copyright (C) 2011-2012 Devin Anderson
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <synthclone/sampleoutputstream.h>

#include "samplefile.h"

using synthclone::SampleOutputStream;

SampleOutputStream::SampleOutputStream(Sample &sample, SampleRate sampleRate,
                                       SampleChannelCount channels,
                                       QObject *parent):
    SampleStream(parent)
{
    file = new SampleFile(sample.getPath(), sampleRate, channels, this);
}

SampleOutputStream::SampleOutputStream(Sample &sample, SampleRate sampleRate,
                                       SampleChannelCount channels, Type type,
                                       SubType subType, EndianType endianType,
                                       QObject *parent):
    SampleStream(parent)
{
    file = new SampleFile(sample.getPath(), sampleRate, channels, type, subType,
                          endianType, this);
}

SampleOutputStream::~SampleOutputStream()
{
    delete file;
}

void
SampleOutputStream::write(const float *buffer, SampleFrameCount frames)
{
    file->write(buffer, frames);
}
