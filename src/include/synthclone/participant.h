#ifndef __SYNTHCLONE_PARTICIPANT_H__
#define __SYNTHCLONE_PARTICIPANT_H__

#include <QtCore/QVariant>

#include <synthclone/context.h>

namespace synthclone {

    /**
     * Participants objects interact with `synthclone`.  Interaction takes place
     * through the Context object that is passed with activate().
     */

    class Participant: public QObject {

        Q_OBJECT

    public:

        /**
         * Links the Participant with the application.
         *
         * @param context
         *   A context object specific to this participant.  The context object
         *   serves as a mediator between this participant and `synthclone`.
         *
         * @param state
         *   If a session is being restored, then the state argument will
         *   contain the return value of getState(), called when the session was
         *   previously loaded; otherwise, the argument is an invalid QVariant.
         */

        virtual void
        activate(Context &context, const QVariant &state=QVariant());

        /**
         * Unlinks the Participant from the application.  After returning from
         * this method, the Context object will be destroyed.
         *
         * @param context
         *   The Context object that was passed to activate().  This object will
         *   be destroyed some time after this call returns.
         */

        virtual void
        deactivate(Context &context);

        /**
         * Gets the author of this Participant.
         *
         * @returns
         *   The author.
         */

        QString
        getAuthor() const;

        /**
         * Gets the major version for this Participant.
         *
         * @returns
         *   The major version.
         */

        int
        getMajorVersion() const;

        /**
         * Gets the minor version for this Participant.
         *
         * @returns
         *   The minor version.
         */

        int
        getMinorVersion() const;

        /**
         * Gets the translated name for this Participant.
         *
         * @returns
         *   The translated name.
         */

        QString
        getName() const;

        /**
         * Gets the revision portion of the Participant version.
         *
         * @returns
         *   The revision.
         */

        int
        getRevision() const;

        /**
         * Gets the state of the Participant.  The state will be passed to the
         * activate() method if the current session is reloaded.  The default
         * implementation returns an invalid QVariant.
         *
         * @returns
         *   The participant state.
         */

        virtual QVariant
        getState() const;

        /**
         * Gets the state of an Effect created by this Participant.  The state
         * should contain all the details necessary to recreate the Effect.  The
         * state will be passed to the restoreEffect() method if the current
         * session is reloaded.
         *
         * @param effect
         *   An effect created by this participant.
         *
         * @returns
         *   The effect state.
         */

        virtual QVariant
        getState(const synthclone::Effect *effect) const;

        /**
         * Gets the state of a Sampler created by this Participant.  The state
         * should contain all the details necessary to recreate the Sampler.
         * The state will be passed to the restoreSampler() method if the
         * current session is reloaded.
         *
         * @param sampler
         *   A sampler created by this participant.
         *
         * @returns
         *   The sampler state.
         */

        virtual QVariant
        getState(const synthclone::Sampler *sampler) const;

        /**
         * Gets the state of a Target created by this Participant.  The state
         * should contain all the details necessary to recreate the Target.  The
         * state will be passed to the restoreTarget() method if the current
         * session is reloaded.
         *
         * @param target
         *   A target created by this participant.
         *
         * @returns
         *   The target state.
         */

        virtual QVariant
        getState(const synthclone::Target *target) const;

        /**
         * Gets the translated summary for this participant.
         *
         * @returns
         *   The translated summary.
         */

        QString
        getSummary() const;

        /**
         * Called to restore an Effect.  The Participant should create the
         * Effect specified by the given state, and append the effect to the
         * session's Effect list using the Context object.
         *
         * @param state
         *   The state of the effect.
         */

        virtual void
        restoreEffect(const QVariant &state);

        /**
         * Called to restore a Sampler.  The Participant should create the
         * Sampler specified by the given state, and set the Sampler using the
         * Context object.
         *
         * @param state
         *   The state of the sampler.
         */

        virtual void
        restoreSampler(const QVariant &state);

        /**
         * Called to restore a Target.  The Participant should create the Target
         * specified by the given state, and append the Target to the session's
         * Target list using the Context object.
         *
         * @param state
         *   The state of the target.
         */

        virtual void
        restoreTarget(const QVariant &state);

    protected:

        /**
         * Constructs a new participant.  This constructor should not be called
         * directly; instead, Participant subclasses should call this
         * constructor in their constructors.
         *
         * @param name
         *   The translated participant name.
         *
         * @param majorVersion
         *   The major version of the participant.
         *
         * @param minorVersion
         *   The minor version of the participant.
         *
         * @param revision
         *   The revision portion of the version of the participant.
         *
         * @param author
         *   The author of the participant.
         *
         * @param summary
         *   A translated summary for the participant.
         *
         * @param parent
         *   The parent object of the new participant.
         */

        Participant(const QString &name, int majorVersion, int minorVersion,
                    int revision, const QString &author, const QString &summary,
                    QObject *parent=0);

        /**
         * Destroys a participant.
         */

        virtual
        ~Participant();

    private:

        QString author;
        int majorVersion;
        int minorVersion;
        QString name;
        int revision;
        QString summary;

    };

}

#endif
