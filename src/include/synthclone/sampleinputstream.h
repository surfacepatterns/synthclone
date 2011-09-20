#ifndef __SYNTHCLONE_SAMPLEINPUTSTREAM_H__
#define __SYNTHCLONE_SAMPLEINPUTSTREAM_H__

#include <synthclone/samplestream.h>

namespace synthclone {

    /**
     * Used to read sample data from a sample file.  The object uses the
     * excellent libsndfile in its implementation, which can be found at
     * http://www.mega-nerd.com/libsndfile/.
     */

    class SampleInputStream: public SampleStream {

        Q_OBJECT

    public:

        /**
         * Constructs a new sample input stream.
         *
         * @param sample
         *   The sample to read data from.
         *
         * @param parent
         *   The parent object of the new stream object.
         */

        explicit
        SampleInputStream(const Sample &sample, QObject *parent=0);

        /**
         * Destroys the stream.
         */

        virtual
        ~SampleInputStream();

        /**
         * Reads data from the stream.
         *
         * @param buffer
         *   A buffer to read data into.  The buffer's size should be greater
         *   than or equal to:
         *   @code frames * channels * sizeof(float) @endcode
         *
         * @param frames
         *   The maximum number of frames to read into the buffer.  If the
         *   stream is positioned less than the amount of frames from the end of
         *   the sample file, then less data will be read into the buffer.
         *
         * @returns
         *   The number of frames read into the buffer.
         */

        SampleFrameCount
        read(float *buffer, SampleFrameCount frames);

    };

}

#endif
