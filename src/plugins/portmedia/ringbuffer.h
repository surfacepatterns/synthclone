/*
 * libsynthclone_portmedia - PortAudio/PortMIDI sampler plugin for `synthclone`
 * Copyright (C) 2012 Devin Anderson
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

#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#include <cassert>
#include <cstdlib>
#include <new>

#include "portaudio/pa_ringbuffer.h"

template<typename T>
class RingBuffer {

public:

    RingBuffer(size_t elementCount)
    {
        assert(elementCount > 0);

        // Get a count that is a power of 2 and is greater than or equal to the
        // given element count.
        size_t realCount;
        for (realCount = 1; (realCount < elementCount) && realCount;
             realCount <<= 1);
        assert(realCount);

        data = malloc(sizeof(T) * realCount);
        if (! data) {
            throw std::bad_alloc();
        }
        ring_buffer_size_t result = PaUtil_InitializeRingBuffer
            (&ringBuffer, sizeof(T), realCount, data);
        assert(result != -1);
    }

    ~RingBuffer()
    {
        free(data);
    }

    void
    flush()
    {
        PaUtil_FlushRingBuffer(&ringBuffer);
    }

    bool
    isReadable() const
    {
        return static_cast<bool>
            (PaUtil_GetRingBufferReadAvailable(&ringBuffer));
    }

    bool
    isWritable() const
    {
        return static_cast<bool>
            (PaUtil_GetRingBufferWriteAvailable(&ringBuffer));
    }

    bool
    read(T &obj)
    {
        return static_cast<bool>(PaUtil_ReadRingBuffer(&ringBuffer, &obj, 1));
    }

    bool
    write(const T &obj)
    {
        return static_cast<bool>(PaUtil_WriteRingBuffer(&ringBuffer, &obj, 1));
    }

private:

    void *data;
    PaUtilRingBuffer ringBuffer;

};

#endif
