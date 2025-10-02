export module synthclone.app:controller;

import synthclone.external.qt;

import :about;

///////////////////////////////////////////////////////////////////////////////
// synthclone::run()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    int
    run(int argc, char** argv)
    {
        ::QApplication application(argc, argv);

        about_view about;

        for (const auto message: about.run(nullptr)) {
            application.processEvents(::QEventLoop::WaitForMoreEvents);
        }

        return 0;
    }

}
