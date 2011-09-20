#ifndef __SYNTHCLONE_SAMPLEOUTPUTSTREAM_H__
#define __SYNTHCLONE_SAMPLEOUTPUTSTREAM_H__

#include <synthclone/samplestream.h>

namespace synthclone {

    /**
     * Used to write sample data to a sample file.  The object uses the
     * excellent libsndfile in its implementation, which can be found at
     * http://www.mega-nerd.com/libsndfile/.
     */

    class SampleOutputStream: public SampleStream {

        Q_OBJECT

    public:

        /**
         * Creates a sample output stream object that writes a .wav file with
         * float values.
         *
         * @param sample
         *   The sample to write to.
         *
         * @param sampleRate
         *   The sample rate to use.
         *
         * @param channels
         *   The channel count.
         *
         * @param parent
         *   The parent object of the new stream.
         */

        SampleOutputStream(Sample &sample, SampleRate sampleRate,
                           SampleChannelCount channels, QObject *parent=0);

        /**
         * Creates a sample output stream object.
         *
         * @param sample
         *   The sample to write to.
         *
         * @param sampleRate
         *   The sample rate to use.
         *
         * @param channels
         *   The channel count.
         *
         * @param type
         *   The format type.
         *
         * @param subType
         *   The format sub-type.
         *
         * @param endianType
         *   The format endian-type.
         *
         * @param parent
         *   The parent object of the new stream.
         */

        SampleOutputStream(Sample &sample, SampleRate sampleRate,
                           SampleChannelCount channels, Type type,
                           SubType subType,
                           EndianType endianType=ENDIANTYPE_FILE,
                           QObject *parent=0);

        /**
         * Destroys the stream.  This will close the stream if it isn't closed.
         */

        virtual
        ~SampleOutputStream();

        /**
         * Writes 'frames' data to the stream.  The data is contained in
         * 'buffer'.
         */

        void
        write(const float *buffer, SampleFrameCount frames);

    private:

        void
        initialize(SampleRate sampleRate, SampleChannelCount channels,
                   int format);

        Sample &sample;

    };

}

#endif
