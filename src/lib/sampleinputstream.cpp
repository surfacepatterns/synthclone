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

#include <synthclone/sampleinputstream.h>

#include "samplefile.h"

using synthclone::SampleInputStream;

SampleInputStream::SampleInputStream(const Sample &sample, QObject *parent):
    SampleStream(parent)
{
    file = new SampleFile(sample.getPath(), this);
}

SampleInputStream::~SampleInputStream()
{
    delete file;
}

synthclone::SampleFrameCount
SampleInputStream::read(float *buffer, SampleFrameCount frames)
{
    return file->read(buffer, frames);
}
