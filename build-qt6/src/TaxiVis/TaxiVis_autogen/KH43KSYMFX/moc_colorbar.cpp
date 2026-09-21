/****************************************************************************
** Meta object code from reading C++ file 'colorbar.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/TaxiVis/util/colorbar.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'colorbar.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN8ColorBarE_t {};
} // unnamed namespace

template <> constexpr inline auto ColorBar::qt_create_metaobjectdata<qt_meta_tag_ZN8ColorBarE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ColorBar",
        "rangeSelectionChanged",
        "",
        "min",
        "max",
        "rangeLimitsChanged",
        "rangelimitsReseted",
        "setColorScale",
        "ColorScale*",
        "colorScale",
        "setRealMinMax",
        "paint",
        "QPainter*",
        "painter",
        "processMouseEvent",
        "QMouseEvent*",
        "event"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'rangeSelectionChanged'
        QtMocHelpers::SignalData<bool(double, double)>(1, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::Double, 3 }, { QMetaType::Double, 4 },
        }}),
        // Signal 'rangeLimitsChanged'
        QtMocHelpers::SignalData<bool(double, double)>(5, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::Double, 3 }, { QMetaType::Double, 4 },
        }}),
        // Signal 'rangelimitsReseted'
        QtMocHelpers::SignalData<bool()>(6, 2, QMC::AccessPublic, QMetaType::Bool),
        // Slot 'setColorScale'
        QtMocHelpers::SlotData<void(ColorScale *)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 8, 9 },
        }}),
        // Slot 'setRealMinMax'
        QtMocHelpers::SlotData<void(double, double)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 3 }, { QMetaType::Double, 4 },
        }}),
        // Slot 'paint'
        QtMocHelpers::SlotData<void(QPainter *)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 12, 13 },
        }}),
        // Slot 'processMouseEvent'
        QtMocHelpers::SlotData<bool(QMouseEvent *)>(14, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { 0x80000000 | 15, 16 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ColorBar, qt_meta_tag_ZN8ColorBarE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ColorBar::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8ColorBarE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8ColorBarE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN8ColorBarE_t>.metaTypes,
    nullptr
} };

void ColorBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ColorBar *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { bool _r = _t->rangeSelectionChanged((*reinterpret_cast<std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 1: { bool _r = _t->rangeLimitsChanged((*reinterpret_cast<std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 2: { bool _r = _t->rangelimitsReseted();
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 3: _t->setColorScale((*reinterpret_cast<std::add_pointer_t<ColorScale*>>(_a[1]))); break;
        case 4: _t->setRealMinMax((*reinterpret_cast<std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2]))); break;
        case 5: _t->paint((*reinterpret_cast<std::add_pointer_t<QPainter*>>(_a[1]))); break;
        case 6: { bool _r = _t->processMouseEvent((*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<bool (ColorBar::*)(double , double )>(_a, &ColorBar::rangeSelectionChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<bool (ColorBar::*)(double , double )>(_a, &ColorBar::rangeLimitsChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<bool (ColorBar::*)()>(_a, &ColorBar::rangelimitsReseted, 2))
            return;
    }
}

const QMetaObject *ColorBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ColorBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8ColorBarE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ColorBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
bool ColorBar::rangeSelectionChanged(double _t1, double _t2)
{
    bool _t0{};
    QMetaObject::activate<bool>(this, &staticMetaObject, 0, std::addressof(_t0), _t1, _t2);
    return _t0;
}

// SIGNAL 1
bool ColorBar::rangeLimitsChanged(double _t1, double _t2)
{
    bool _t0{};
    QMetaObject::activate<bool>(this, &staticMetaObject, 1, std::addressof(_t0), _t1, _t2);
    return _t0;
}

// SIGNAL 2
bool ColorBar::rangelimitsReseted()
{
    bool _t0{};
    QMetaObject::activate<bool>(this, &staticMetaObject, 2, std::addressof(_t0));
    return _t0;
}
QT_WARNING_POP
