/****************************************************************************
** Meta object code from reading C++ file 'geographicalviewwidget.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/TaxiVis/geographicalviewwidget.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'geographicalviewwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN22GeographicalViewWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto GeographicalViewWidget::qt_create_metaobjectdata<qt_meta_tag_ZN22GeographicalViewWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "GeographicalViewWidget",
        "mapSelectionChanged",
        "",
        "datasetUpdated",
        "stepBack",
        "stepForward",
        "updateEditingAnchors"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'mapSelectionChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'datasetUpdated'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'stepBack'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'stepForward'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'updateEditingAnchors'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<GeographicalViewWidget, qt_meta_tag_ZN22GeographicalViewWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject GeographicalViewWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QMapWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN22GeographicalViewWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN22GeographicalViewWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN22GeographicalViewWidgetE_t>.metaTypes,
    nullptr
} };

void GeographicalViewWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<GeographicalViewWidget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->mapSelectionChanged(); break;
        case 1: _t->datasetUpdated(); break;
        case 2: _t->stepBack(); break;
        case 3: _t->stepForward(); break;
        case 4: _t->updateEditingAnchors(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (GeographicalViewWidget::*)()>(_a, &GeographicalViewWidget::mapSelectionChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (GeographicalViewWidget::*)()>(_a, &GeographicalViewWidget::datasetUpdated, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (GeographicalViewWidget::*)()>(_a, &GeographicalViewWidget::stepBack, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (GeographicalViewWidget::*)()>(_a, &GeographicalViewWidget::stepForward, 3))
            return;
    }
}

const QMetaObject *GeographicalViewWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GeographicalViewWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN22GeographicalViewWidgetE_t>.strings))
        return static_cast<void*>(this);
    return QMapWidget::qt_metacast(_clname);
}

int GeographicalViewWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMapWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void GeographicalViewWidget::mapSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void GeographicalViewWidget::datasetUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void GeographicalViewWidget::stepBack()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void GeographicalViewWidget::stepForward()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
