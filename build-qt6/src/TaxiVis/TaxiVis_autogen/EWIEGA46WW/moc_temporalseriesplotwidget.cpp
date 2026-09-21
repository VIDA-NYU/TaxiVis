/****************************************************************************
** Meta object code from reading C++ file 'temporalseriesplotwidget.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/TaxiVis/temporalseriesplotwidget.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'temporalseriesplotwidget.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN24TemporalSeriesPlotWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto TemporalSeriesPlotWidget::qt_create_metaobjectdata<qt_meta_tag_ZN24TemporalSeriesPlotWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "TemporalSeriesPlotWidget",
        "timeIntervalChanged",
        "",
        "updateNumBins",
        "updatePlotAttrib",
        "updateYRange",
        "min",
        "max",
        "selectionChanged",
        "mousePress",
        "mouseWheel"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'timeIntervalChanged'
        QtMocHelpers::SignalData<void(QDateTime, QDateTime)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QDateTime, 2 }, { QMetaType::QDateTime, 2 },
        }}),
        // Slot 'updateNumBins'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'updatePlotAttrib'
        QtMocHelpers::SlotData<void(QString)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 2 },
        }}),
        // Slot 'updateYRange'
        QtMocHelpers::SlotData<void(float, float)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Float, 6 }, { QMetaType::Float, 7 },
        }}),
        // Slot 'selectionChanged'
        QtMocHelpers::SlotData<void(QDateTime, QDateTime)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QDateTime, 2 }, { QMetaType::QDateTime, 2 },
        }}),
        // Slot 'mousePress'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'mouseWheel'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<TemporalSeriesPlotWidget, qt_meta_tag_ZN24TemporalSeriesPlotWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject TemporalSeriesPlotWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN24TemporalSeriesPlotWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN24TemporalSeriesPlotWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN24TemporalSeriesPlotWidgetE_t>.metaTypes,
    nullptr
} };

void TemporalSeriesPlotWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<TemporalSeriesPlotWidget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->timeIntervalChanged((*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[2]))); break;
        case 1: _t->updateNumBins(); break;
        case 2: _t->updatePlotAttrib((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->updateYRange((*reinterpret_cast<std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<float>>(_a[2]))); break;
        case 4: _t->selectionChanged((*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[2]))); break;
        case 5: _t->mousePress(); break;
        case 6: _t->mouseWheel(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (TemporalSeriesPlotWidget::*)(QDateTime , QDateTime )>(_a, &TemporalSeriesPlotWidget::timeIntervalChanged, 0))
            return;
    }
}

const QMetaObject *TemporalSeriesPlotWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TemporalSeriesPlotWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN24TemporalSeriesPlotWidgetE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int TemporalSeriesPlotWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void TemporalSeriesPlotWidget::timeIntervalChanged(QDateTime _t1, QDateTime _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}
QT_WARNING_POP
