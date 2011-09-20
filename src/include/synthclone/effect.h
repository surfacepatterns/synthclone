#ifndef __SYNTHCLONE_EFFECT_H__
#define __SYNTHCLONE_EFFECT_H__

#include <synthclone/component.h>
#include <synthclone/sampleinputstream.h>
#include <synthclone/sampleoutputstream.h>
#include <synthclone/zone.h>

namespace synthclone {

    /**
     * Component capable of altering samples in some way.
     */

    class Effect: public Component {

        Q_OBJECT

    public:

        /**
         * Called to apply this Effect to audio data.  The
         * Component::progressChanged() and Component::statusChanged() signals
         * should be used to indicate progress in applying the Effect.
         *
         * @param zone
         *   The Zone for which the Effect is being applied.  An Effect can use
         *   the Zone with its ZonePropertyMap to get ZoneProperty values that
         *   are specific to the zone.
         *
         * @param inputStream
         *   Contains the audio data to which this Effect should be applied.
         *
         * @param outputStream
         *   Processed audio data is written to this stream.
         *
         * @note
         *   This method will not be called by the main thread.  Effect
         *   processing is handled in a separate thread in an attempt to make
         *   sure that the GUI is responsive even when effects are being
         *   applied.
         */

        virtual void
        process(const Zone &zone, SampleInputStream &inputStream,
                SampleOutputStream &outputStream) = 0;

    protected:

        /**
         * Constructs a new Effect object.  This constructor cannot be called
         * directly; instead, subclasses should call this constructor in their
         * constructors.
         *
         * @param name
         *   The initial name for the effect.
         *
         * @param parent
         *   The parent object of the effect.
         */

        explicit
        Effect(const QString &name, QObject *parent=0);

        virtual
        ~Effect();

    };

}

#endif
