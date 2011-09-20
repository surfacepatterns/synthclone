#ifndef __ZONETABLEDELEGATE_H__
#define __ZONETABLEDELEGATE_H__

#include <QtGui/QStyledItemDelegate>

#include <synthclone/types.h>

class ZoneTableDelegate: public QStyledItemDelegate {

    Q_OBJECT

public:

    explicit
    ZoneTableDelegate(QObject *parent=0);

    ~ZoneTableDelegate();

    QWidget *
    createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                 const QModelIndex &index) const;

    QModelIndex
    getEditIndex() const;

    void
    setEditorData(QWidget *editor, const QModelIndex &index) const;

    void
    setModelData(QWidget *editor, QAbstractItemModel *model,
                 const QModelIndex &index) const;

    void
    updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const;

signals:

    void
    aftertouchChangeRequest(int index, synthclone::MIDIData aftertouch) const;

    void
    channelChangeRequest(int index, synthclone::MIDIData channel) const;

    void
    controlValueChangeRequest(int index, synthclone::MIDIData control,
                              synthclone::MIDIData value) const;

    void
    noteChangeRequest(int index, synthclone::MIDIData note) const;

    void
    releaseTimeChangeRequest(int index,
                             synthclone::SampleTime releaseTime) const;

    void
    sampleTimeChangeRequest(int index, synthclone::SampleTime sampleTime) const;

    void
    velocityChangeRequest(int index, synthclone::MIDIData velocity) const;

private:

    mutable QModelIndex editIndex;

};

#endif
