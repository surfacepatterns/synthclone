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

#include <cassert>
#include <cerrno>
#include <cstring>

#include <QtCore/QDebug>

#include <synthclone/error.h>
#include <synthclone/samplestream.h>
#include <synthclone/util.h>

#include "samplefile.h"

using synthclone::SampleFile;

SampleFile::SampleFile(const QString &path, QObject *parent):
    QObject(parent)
{
    info.format = 0;
    QByteArray pathBytes = path.toLocal8Bit();
    handle = sf_open(pathBytes.data(), SFM_READ, &info);
    if (! handle) {
        throw Error(std::strerror(errno));
    }
    closed = false;
    writeMode = false;
}

SampleFile::SampleFile(const QString &path, SampleRate sampleRate,
                       SampleChannelCount channels, QObject *parent):
    QObject(parent)
{
    initializeWriteMode(path, sampleRate, channels, SampleStream::TYPE_WAV,
                        SampleStream::SUBTYPE_FLOAT,
                        SampleStream::ENDIANTYPE_FILE);
}

SampleFile::SampleFile(const QString &path, SampleRate sampleRate,
                       SampleChannelCount channels, SampleStream::Type type,
                       SampleStream::SubType subType,
                       SampleStream::EndianType endianType, QObject *parent):
    QObject(parent)
{
    initializeWriteMode(path, sampleRate, channels, type, subType, endianType);
}

SampleFile::~SampleFile()
{
    if (! closed) {
        try {
            close();
        } catch (synthclone::Error &e) {
            qWarning() << e.getMessage();
        }
    }
}

void
SampleFile::close()
{
    if (writeMode && (! framesWritten)) {
        // Hack: Write one silent frame to the buffer if no samples have been
        // written to the buffer.
        SampleChannelCount channels = getChannels();
        float *samples = new float[channels];
        QScopedArrayPointer<float> samplesPtr(samples);
        for (SampleChannelCount i = 0; i < channels; i++) {
            samples[i] = 0.0;
        }
        write(samples, 1);
    }
    int result = sf_close(handle);
    if (result) {
        throw synthclone::Error(sf_error_number(result));
    }
    closed = true;
}

synthclone::SampleChannelCount
SampleFile::getChannels() const
{
    return static_cast<SampleChannelCount>(info.channels);
}

synthclone::SampleStream::EndianType
SampleFile::getEndianType() const
{
    switch (info.format & SF_FORMAT_ENDMASK) {
    case SF_ENDIAN_FILE:
        return SampleStream::ENDIANTYPE_FILE;
    case SF_ENDIAN_LITTLE:
        return SampleStream::ENDIANTYPE_LITTLE;
    case SF_ENDIAN_BIG:
        return SampleStream::ENDIANTYPE_BIG;
    default:
        ;
    }
    return SampleStream::ENDIANTYPE_CPU;
}

synthclone::SampleFrameCount
SampleFile::getFrames() const
{
    return static_cast<SampleFrameCount>(info.frames);
}

synthclone::SampleRate
SampleFile::getSampleRate() const
{
    return static_cast<SampleRate>(info.samplerate);
}

synthclone::SampleStream::SubType
SampleFile::getSubType() const
{
    switch (info.format & SF_FORMAT_SUBMASK) {
    case SF_FORMAT_PCM_S8:
        return SampleStream::SUBTYPE_PCM_S8;
    case SF_FORMAT_PCM_U8:
        return SampleStream::SUBTYPE_PCM_U8;
    case SF_FORMAT_PCM_16:
        return SampleStream::SUBTYPE_PCM_16;
    case SF_FORMAT_PCM_24:
        return SampleStream::SUBTYPE_PCM_24;
    case SF_FORMAT_PCM_32:
        return SampleStream::SUBTYPE_PCM_32;
    case SF_FORMAT_FLOAT:
        return SampleStream::SUBTYPE_FLOAT;
    case SF_FORMAT_DOUBLE:
        return SampleStream::SUBTYPE_DOUBLE;
    case SF_FORMAT_ULAW:
        return SampleStream::SUBTYPE_ULAW;
    case SF_FORMAT_ALAW:
        return SampleStream::SUBTYPE_ALAW;
    case SF_FORMAT_IMA_ADPCM:
        return SampleStream::SUBTYPE_IMA_ADPCM;
    case SF_FORMAT_MS_ADPCM:
        return SampleStream::SUBTYPE_MS_ADPCM;
    case SF_FORMAT_GSM610:
        return SampleStream::SUBTYPE_GSM610;
    case SF_FORMAT_VOX_ADPCM:
        return SampleStream::SUBTYPE_VOX_ADPCM;
    case SF_FORMAT_G721_32:
        return SampleStream::SUBTYPE_G721_32;
    case SF_FORMAT_G723_24:
        return SampleStream::SUBTYPE_G723_24;
    case SF_FORMAT_G723_40:
        return SampleStream::SUBTYPE_G723_40;
    case SF_FORMAT_DWVW_12:
        return SampleStream::SUBTYPE_DWVW_12;
    case SF_FORMAT_DWVW_16:
        return SampleStream::SUBTYPE_DWVW_16;
    case SF_FORMAT_DWVW_24:
        return SampleStream::SUBTYPE_DWVW_24;
    case SF_FORMAT_DWVW_N:
        return SampleStream::SUBTYPE_DWVW_N;
    case SF_FORMAT_DPCM_8:
        return SampleStream::SUBTYPE_DPCM_8;
    case SF_FORMAT_DPCM_16:
        return SampleStream::SUBTYPE_DPCM_16;
    case SF_FORMAT_VORBIS:
        return SampleStream::SUBTYPE_VORBIS;
    default:
        ;
    }
    return SampleStream::SUBTYPE_UNKNOWN;
}

synthclone::SampleStream::Type
SampleFile::getType() const
{
    switch (info.format & SF_FORMAT_TYPEMASK) {
    case SF_FORMAT_AIFF:
        return SampleStream::TYPE_AIFF;
    case SF_FORMAT_AU:
        return SampleStream::TYPE_AU;
    case SF_FORMAT_AVR:
        return SampleStream::TYPE_AVR;
    case SF_FORMAT_CAF:
        return SampleStream::TYPE_CAF;
    case SF_FORMAT_FLAC:
        return SampleStream::TYPE_FLAC;
    case SF_FORMAT_HTK:
        return SampleStream::TYPE_HTK;
    case SF_FORMAT_IRCAM:
        return SampleStream::TYPE_IRCAM;
    case SF_FORMAT_MAT4:
        return SampleStream::TYPE_MAT4;
    case SF_FORMAT_MAT5:
        return SampleStream::TYPE_MAT5;
    case SF_FORMAT_MPC2K:
        return SampleStream::TYPE_MPC2K;
    case SF_FORMAT_NIST:
        return SampleStream::TYPE_NIST;
    case SF_FORMAT_OGG:
        return SampleStream::TYPE_OGG;
    case SF_FORMAT_PAF:
        return SampleStream::TYPE_PAF;
    case SF_FORMAT_PVF:
        return SampleStream::TYPE_PVF;
    case SF_FORMAT_RAW:
        return SampleStream::TYPE_RAW;
    case SF_FORMAT_RF64:
        return SampleStream::TYPE_RF64;
    case SF_FORMAT_SD2:
        return SampleStream::TYPE_SD2;
    case SF_FORMAT_SDS:
        return SampleStream::TYPE_SDS;
    case SF_FORMAT_SVX:
        return SampleStream::TYPE_SVX;
    case SF_FORMAT_VOC:
        return SampleStream::TYPE_VOC;
    case SF_FORMAT_W64:
        return SampleStream::TYPE_W64;
    case SF_FORMAT_WAV:
        return SampleStream::TYPE_WAV;
    case SF_FORMAT_WAVEX:
        return SampleStream::TYPE_WAVEX;
    case SF_FORMAT_WVE:
        return SampleStream::TYPE_WVE;
    case SF_FORMAT_XI:
        return SampleStream::TYPE_XI;
    default:
        ;
    }
    return SampleStream::TYPE_UNKNOWN;
}

void
SampleFile::initializeWriteMode(const QString &path, SampleRate sampleRate,
                                SampleChannelCount channels,
                                SampleStream::Type type,
                                SampleStream::SubType subType,
                                SampleStream::EndianType endianType)
{
    int format = 0;

    switch (type) {
    case SampleStream::TYPE_AIFF:
        format |= SF_FORMAT_AIFF;
        break;
    case SampleStream::TYPE_AU:
        format |= SF_FORMAT_AU;
        break;
    case SampleStream::TYPE_AVR:
        format |= SF_FORMAT_AVR;
        break;
    case SampleStream::TYPE_CAF:
        format |= SF_FORMAT_CAF;
        break;
    case SampleStream::TYPE_FLAC:
        format |= SF_FORMAT_FLAC;
        break;
    case SampleStream::TYPE_HTK:
        format |= SF_FORMAT_HTK;
        break;
    case SampleStream::TYPE_IRCAM:
        format |= SF_FORMAT_IRCAM;
        break;
    case SampleStream::TYPE_MAT4:
        format |= SF_FORMAT_MAT4;
        break;
    case SampleStream::TYPE_MAT5:
        format |= SF_FORMAT_MAT5;
        break;
    case SampleStream::TYPE_MPC2K:
        format |= SF_FORMAT_MPC2K;
        break;
    case SampleStream::TYPE_NIST:
        format |= SF_FORMAT_NIST;
        break;
    case SampleStream::TYPE_OGG:
        format |= SF_FORMAT_OGG;
        break;
    case SampleStream::TYPE_PAF:
        format |= SF_FORMAT_PAF;
        break;
    case SampleStream::TYPE_PVF:
        format |= SF_FORMAT_PVF;
        break;
    case SampleStream::TYPE_RAW:
        format |= SF_FORMAT_RAW;
        break;
    case SampleStream::TYPE_RF64:
        format |= SF_FORMAT_RF64;
        break;
    case SampleStream::TYPE_SD2:
        format |= SF_FORMAT_SD2;
        break;
    case SampleStream::TYPE_SDS:
        format |= SF_FORMAT_SDS;
        break;
    case SampleStream::TYPE_SVX:
        format |= SF_FORMAT_SVX;
        break;
    case SampleStream::TYPE_VOC:
        format |= SF_FORMAT_VOC;
        break;
    case SampleStream::TYPE_W64:
        format |= SF_FORMAT_W64;
        break;
    case SampleStream::TYPE_WAV:
        format |= SF_FORMAT_WAV;
        break;
    case SampleStream::TYPE_WAVEX:
        format |= SF_FORMAT_WAVEX;
        break;
    case SampleStream::TYPE_WVE:
        format |= SF_FORMAT_WVE;
        break;
    case SampleStream::TYPE_XI:
        format |= SF_FORMAT_XI;
        break;
    default:
        CONFIRM(false, tr("invalid sample stream type"));
    }

    switch (subType) {
    case SampleStream::SUBTYPE_PCM_S8:
        format |= SF_FORMAT_PCM_S8;
        break;
    case SampleStream::SUBTYPE_PCM_U8:
        format |= SF_FORMAT_PCM_U8;
        break;
    case SampleStream::SUBTYPE_PCM_16:
        format |= SF_FORMAT_PCM_16;
        break;
    case SampleStream::SUBTYPE_PCM_24:
        format |= SF_FORMAT_PCM_24;
        break;
    case SampleStream::SUBTYPE_PCM_32:
        format |= SF_FORMAT_PCM_32;
        break;
    case SampleStream::SUBTYPE_FLOAT:
        format |= SF_FORMAT_FLOAT;
        break;
    case SampleStream::SUBTYPE_DOUBLE:
        format |= SF_FORMAT_DOUBLE;
        break;
    case SampleStream::SUBTYPE_ULAW:
        format |= SF_FORMAT_ULAW;
        break;
    case SampleStream::SUBTYPE_ALAW:
        format |= SF_FORMAT_ALAW;
        break;
    case SampleStream::SUBTYPE_IMA_ADPCM:
        format |= SF_FORMAT_IMA_ADPCM;
        break;
    case SampleStream::SUBTYPE_MS_ADPCM:
        format |= SF_FORMAT_MS_ADPCM;
        break;
    case SampleStream::SUBTYPE_GSM610:
        format |= SF_FORMAT_GSM610;
        break;
    case SampleStream::SUBTYPE_VOX_ADPCM:
        format |= SF_FORMAT_VOX_ADPCM;
        break;
    case SampleStream::SUBTYPE_G721_32:
        format |= SF_FORMAT_G721_32;
        break;
    case SampleStream::SUBTYPE_G723_24:
        format |= SF_FORMAT_G723_24;
        break;
    case SampleStream::SUBTYPE_G723_40:
        format |= SF_FORMAT_G723_40;
        break;
    case SampleStream::SUBTYPE_DWVW_12:
        format |= SF_FORMAT_DWVW_12;
        break;
    case SampleStream::SUBTYPE_DWVW_16:
        format |= SF_FORMAT_DWVW_16;
        break;
    case SampleStream::SUBTYPE_DWVW_24:
        format |= SF_FORMAT_DWVW_24;
        break;
    case SampleStream::SUBTYPE_DWVW_N:
        format |= SF_FORMAT_DWVW_N;
        break;
    case SampleStream::SUBTYPE_DPCM_8:
        format |= SF_FORMAT_DPCM_8;
        break;
    case SampleStream::SUBTYPE_DPCM_16:
        format |= SF_FORMAT_DPCM_16;
        break;
    case SampleStream::SUBTYPE_VORBIS:
        format |= SF_FORMAT_VORBIS;
        break;
    default:
        CONFIRM(false, tr("invalid sample stream sub-type"));
    }

    switch (endianType) {
    case SampleStream::ENDIANTYPE_FILE:
        format |= SF_ENDIAN_FILE;
        break;
    case SampleStream::ENDIANTYPE_LITTLE:
        format |= SF_ENDIAN_LITTLE;
        break;
    case SampleStream::ENDIANTYPE_BIG:
        format |= SF_ENDIAN_BIG;
        break;
    case SampleStream::ENDIANTYPE_CPU:
        format |= SF_ENDIAN_CPU;
        break;
    default:
        CONFIRM(false, tr("invalid sample stream  endian type"));
    }

    info.channels = static_cast<int>(channels);
    info.format = format;
    info.samplerate = static_cast<int>(sampleRate);
    if (! sf_format_check(&info)) {
        throw Error(tr("format is not supported"));
    }
    QByteArray pathBytes = path.toLocal8Bit();
    handle = sf_open(pathBytes.data(), SFM_WRITE, &info);
    if (! handle) {
        throw Error(std::strerror(errno));
    }
    closed = false;
    framesWritten = false;
    writeMode = true;
}

bool
SampleFile::isClosed() const
{
    return closed;
}

synthclone::SampleFrameCount
SampleFile::read(float *buffer, SampleFrameCount frames)
{
    CONFIRM(buffer, tr("buffer is set to NULL"));
    CONFIRM(frames > 0, tr("'%1': invalid frames value").arg(frames));

    sf_count_t readFrames =
        sf_readf_float(handle, buffer, static_cast<sf_count_t>(frames));
    if (readFrames <= 0) {
        int errorNumber = sf_error(handle);
        if (errorNumber != SF_ERR_NO_ERROR) {
            throw synthclone::Error(sf_error_number(errorNumber));
        }
    }
    return static_cast<SampleFrameCount>(readFrames);
}

synthclone::SampleFrameCount
SampleFile::seek(SampleFrameCount frames, SampleStream::Offset offset)
{
    int whence;
    switch (offset) {
    case SampleStream::OFFSET_CURRENT:
        whence = SEEK_CUR;
        break;
    case SampleStream::OFFSET_END:
        whence = SEEK_END;
        break;
    case SampleStream::OFFSET_START:
    default:
        whence = SEEK_SET;
    }
    sf_count_t newLocation =
        sf_seek(handle, static_cast<sf_count_t>(frames), whence);
    if (newLocation == -1) {
        throw Error(sf_strerror(handle));
    }
    return static_cast<SampleFrameCount>(newLocation);
}

void
SampleFile::write(const float *buffer, SampleFrameCount frames)
{
    CONFIRM(buffer, tr("buffer is set to NULL"));
    CONFIRM(frames > 0, tr("'%1': invalid frames value").arg(frames));

    sf_count_t n = static_cast<sf_count_t>(frames);
    if (sf_writef_float(handle, buffer, n) != n) {
        int errorNumber = sf_error(handle);
        assert(errorNumber != SF_ERR_NO_ERROR);
        throw synthclone::Error(sf_error_number(errorNumber));
    }
    framesWritten = true;
}
