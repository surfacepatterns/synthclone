/*
 * libsynthclone - a plugin API for `synthclone`
 * Copyright (C) 2011 Devin Anderson
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

#include <cassert>
#include <cerrno>
#include <cstring>

#include <synthclone/error.h>
#include <synthclone/sampleoutputstream.h>
#include <synthclone/util.h>

using synthclone::SampleOutputStream;

SampleOutputStream::SampleOutputStream(Sample &sample, SampleRate sampleRate,
                                       SampleChannelCount channels,
                                       QObject *parent):
    SampleStream(parent),
    sample(sample)
{
    initialize(sampleRate, channels, TYPE_WAV | SUBTYPE_FLOAT);
}

SampleOutputStream::SampleOutputStream(Sample &sample, SampleRate sampleRate,
                                       SampleChannelCount channels, Type type,
                                       SubType subType, EndianType endianType,
                                       QObject *parent):
    SampleStream(parent),
    sample(sample)
{
    initialize(sampleRate, channels, type | subType | endianType);
}

SampleOutputStream::~SampleOutputStream()
{
    // Empty
}

void
SampleOutputStream::initialize(SampleRate sampleRate,
                               SampleChannelCount sampleChannelCount,
                               int format)
{
    info.channels = static_cast<int>(sampleChannelCount);
    info.format = format;
    info.samplerate = static_cast<int>(sampleRate);
    if (! sf_format_check(&info)) {
        throw Error(tr("format is not supported"));
    }
    handle = sf_open(sample.path.toLocal8Bit().data(), SFM_WRITE, &info);
    if (! handle) {
        throw Error(std::strerror(errno));
    }
}

void
SampleOutputStream::write(const float *buffer, SampleFrameCount frames)
{
    CONFIRM(buffer, tr("buffer is set to NULL"));
    CONFIRM(frames > 0, tr("'%1': invalid frames value").arg(frames));

    sf_count_t n = static_cast<sf_count_t>(frames);
    if (sf_writef_float(handle, buffer, n) != n) {
        int errorNumber = sf_error(handle);
        assert(errorNumber != SF_ERR_NO_ERROR);
        throw synthclone::Error(sf_error_number(errorNumber));
    }
}
