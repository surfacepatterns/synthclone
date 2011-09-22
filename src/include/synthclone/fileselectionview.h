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

#ifndef __SYNTHCLONE_FILESELECTIONVIEW_H__
#define __SYNTHCLONE_FILESELECTIONVIEW_H__

#include <synthclone/view.h>

namespace synthclone {

    /**
     * A utility view class that handles the selection of files and
     * directories.
     *
     * @note
     *   The current implementation is a thin wrapper around QFileDialog.
     */

    class FileSelectionView: public View {

        Q_OBJECT

    public:

        /**
         * Specifies whether the view is being used to open or save a
         * filesystem object.
         */

        enum Operation {
            OPERATION_OPEN,
            OPERATION_SAVE
        };

        /**
         * Specifies the types of filesystem objects the user may select.
         */

        enum SelectionFilter {
            SELECTIONFILTER_ANY_FILE,
            SELECTIONFILTER_DIRECTORY,
            SELECTIONFILTER_EXISTING_FILE,
            SELECTIONFILTER_EXISTING_FILES
        };

        /**
         * Constructs a new FileSelectionView object.
         *
         * @param parent
         *   The parent object of the FileSelectionView object.
         */

        explicit
        FileSelectionView(QObject *parent=0);

        /**
         * Destroys a FileSelectionView object.
         */

        ~FileSelectionView();

        /**
         * Gets a flag indicating whether or not files are visible in the view.
         *
         * @returns
         *   The flag.
         */

        bool
        areFilesVisible() const;

        /**
         * Gets the current displayed directory.
         *
         * @returns
         *   The current displayed directory.
         */

        QString
        getDirectory() const;

        /**
         * Gets the current FileSelectionView::Operation.
         *
         * @returns
         *   The current FileSelectionView::Operation.
         */

        Operation
        getOperation() const;

        /**
         * Gets the selection filter for this view.
         *
         * @returns
         *   The selection filter.
         */

        SelectionFilter
        getSelectionFilter() const;

        /**
         * Gets the window title.
         *
         * @returns
         *   The window title.
         */

        QString
        getTitle() const;

    public slots:

        /**
         * Sets the directory to be displayed.
         *
         * @param directory
         *   The directory.
         */

        void
        setDirectory(const QString &directory);

        /**
         * Sets a flag indicating whether or not to show files in the view.  If
         * the 'visible' argument is set to false, then the selection filter
         * will be set to FileSelectionView::SELECTIONFILTER_DIRECTORY when
         * this method returns.
         *
         * @param visible
         *   The flag.
         */

        void
        setFilesVisible(bool visible);

        /**
         * Sets the FileSelectionView::Operation for the view.
         *
         * @param operation
         *   The FileSelectionView::Operation.
         */

        void
        setOperation(Operation operation);

        /**
         * Sets the FileSelectionView::SelectionFilter for this view.
         *
         * @param filter
         *   The FileSelectionView::SelectionFilter.
         */

        void
        setSelectionFilter(SelectionFilter filter);

        /**
         * Sets the window title.
         *
         * @param title
         *   The title.
         */

        void
        setTitle(const QString &title);

    signals:

        /**
         * Emitted when the user selects a path or path(s).
         *
         * @param paths
         *   The selected paths.
         */

        void
        pathsSelected(const QStringList &paths);

    private slots:

        void
        handleDialogAccept();

        void
        handleDialogReject();

    };

}

#endif
