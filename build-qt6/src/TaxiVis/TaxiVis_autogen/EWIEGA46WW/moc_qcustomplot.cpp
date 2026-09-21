/****************************************************************************
** Meta object code from reading C++ file 'qcustomplot.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/TaxiVis/qcustomplot.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qcustomplot.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN12QCPLayerableE_t {};
} // unnamed namespace

template <> constexpr inline auto QCPLayerable::qt_create_metaobjectdata<qt_meta_tag_ZN12QCPLayerableE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QCPLayerable"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QCPLayerable, qt_meta_tag_ZN12QCPLayerableE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QCPLayerable::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12QCPLayerableE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12QCPLayerableE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12QCPLayerableE_t>.metaTypes,
    nullptr
} };

void QCPLayerable::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QCPLayerable *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *QCPLayerable::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCPLayerable::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12QCPLayerableE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int QCPLayerable::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN20QCPAbstractPlottableE_t {};
} // unnamed namespace

template <> constexpr inline auto QCPAbstractPlottable::qt_create_metaobjectdata<qt_meta_tag_ZN20QCPAbstractPlottableE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QCPAbstractPlottable",
        "selectionChanged",
        "",
        "selected"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'selectionChanged'
        QtMocHelpers::SignalData<void(bool)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 3 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QCPAbstractPlottable, qt_meta_tag_ZN20QCPAbstractPlottableE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QCPAbstractPlottable::staticMetaObject = { {
    QMetaObject::SuperData::link<QCPLayerable::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20QCPAbstractPlottableE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20QCPAbstractPlottableE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN20QCPAbstractPlottableE_t>.metaTypes,
    nullptr
} };

void QCPAbstractPlottable::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QCPAbstractPlottable *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->selectionChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (QCPAbstractPlottable::*)(bool )>(_a, &QCPAbstractPlottable::selectionChanged, 0))
            return;
    }
}

const QMetaObject *QCPAbstractPlottable::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCPAbstractPlottable::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20QCPAbstractPlottableE_t>.strings))
        return static_cast<void*>(this);
    return QCPLayerable::qt_metacast(_clname);
}

int QCPAbstractPlottable::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPLayerable::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void QCPAbstractPlottable::selectionChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}
namespace {
struct qt_meta_tag_ZN8QCPGraphE_t {};
} // unnamed namespace

template <> constexpr inline auto QCPGraph::qt_create_metaobjectdata<qt_meta_tag_ZN8QCPGraphE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QCPGraph",
        "LineStyle",
        "lsNone",
        "lsLine",
        "lsStepLeft",
        "lsStepRight",
        "lsStepCenter",
        "lsImpulse",
        "ErrorType",
        "etNone",
        "etKey",
        "etValue",
        "etBoth"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'LineStyle'
        QtMocHelpers::EnumData<enum LineStyle>(1, 1, QMC::EnumFlags{}).add({
            {    2, LineStyle::lsNone },
            {    3, LineStyle::lsLine },
            {    4, LineStyle::lsStepLeft },
            {    5, LineStyle::lsStepRight },
            {    6, LineStyle::lsStepCenter },
            {    7, LineStyle::lsImpulse },
        }),
        // enum 'ErrorType'
        QtMocHelpers::EnumData<enum ErrorType>(8, 8, QMC::EnumFlags{}).add({
            {    9, ErrorType::etNone },
            {   10, ErrorType::etKey },
            {   11, ErrorType::etValue },
            {   12, ErrorType::etBoth },
        }),
    };
    return QtMocHelpers::metaObjectData<QCPGraph, qt_meta_tag_ZN8QCPGraphE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QCPGraph::staticMetaObject = { {
    QMetaObject::SuperData::link<QCPAbstractPlottable::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8QCPGraphE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8QCPGraphE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN8QCPGraphE_t>.metaTypes,
    nullptr
} };

void QCPGraph::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QCPGraph *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *QCPGraph::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCPGraph::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8QCPGraphE_t>.strings))
        return static_cast<void*>(this);
    return QCPAbstractPlottable::qt_metacast(_clname);
}

int QCPGraph::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractPlottable::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN8QCPCurveE_t {};
} // unnamed namespace

template <> constexpr inline auto QCPCurve::qt_create_metaobjectdata<qt_meta_tag_ZN8QCPCurveE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QCPCurve"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QCPCurve, qt_meta_tag_ZN8QCPCurveE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QCPCurve::staticMetaObject = { {
    QMetaObject::SuperData::link<QCPAbstractPlottable::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8QCPCurveE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8QCPCurveE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN8QCPCurveE_t>.metaTypes,
    nullptr
} };

void QCPCurve::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QCPCurve *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *QCPCurve::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCPCurve::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8QCPCurveE_t>.strings))
        return static_cast<void*>(this);
    return QCPAbstractPlottable::qt_metacast(_clname);
}

int QCPCurve::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractPlottable::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN7QCPBarsE_t {};
} // unnamed namespace

template <> constexpr inline auto QCPBars::qt_create_metaobjectdata<qt_meta_tag_ZN7QCPBarsE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QCPBars"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QCPBars, qt_meta_tag_ZN7QCPBarsE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QCPBars::staticMetaObject = { {
    QMetaObject::SuperData::link<QCPAbstractPlottable::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7QCPBarsE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7QCPBarsE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN7QCPBarsE_t>.metaTypes,
    nullptr
} };

void QCPBars::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QCPBars *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *QCPBars::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCPBars::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7QCPBarsE_t>.strings))
        return static_cast<void*>(this);
    return QCPAbstractPlottable::qt_metacast(_clname);
}

int QCPBars::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractPlottable::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN17QCPStatisticalBoxE_t {};
} // unnamed namespace

template <> constexpr inline auto QCPStatisticalBox::qt_create_metaobjectdata<qt_meta_tag_ZN17QCPStatisticalBoxE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QCPStatisticalBox"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QCPStatisticalBox, qt_meta_tag_ZN17QCPStatisticalBoxE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QCPStatisticalBox::staticMetaObject = { {
    QMetaObject::SuperData::link<QCPAbstractPlottable::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17QCPStatisticalBoxE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17QCPStatisticalBoxE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17QCPStatisticalBoxE_t>.metaTypes,
    nullptr
} };

void QCPStatisticalBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QCPStatisticalBox *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *QCPStatisticalBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCPStatisticalBox::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17QCPStatisticalBoxE_t>.strings))
        return static_cast<void*>(this);
    return QCPAbstractPlottable::qt_metacast(_clname);
}

int QCPStatisticalBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractPlottable::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN15QCPAbstractItemE_t {};
} // unnamed namespace

template <> constexpr inline auto QCPAbstractItem::qt_create_metaobjectdata<qt_meta_tag_ZN15QCPAbstractItemE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QCPAbstractItem",
        "selectionChanged",
        "",
        "selected"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'selectionChanged'
        QtMocHelpers::SignalData<void(bool)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 3 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QCPAbstractItem, qt_meta_tag_ZN15QCPAbstractItemE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QCPAbstractItem::staticMetaObject = { {
    QMetaObject::SuperData::link<QCPLayerable::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15QCPAbstractItemE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15QCPAbstractItemE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15QCPAbstractItemE_t>.metaTypes,
    nullptr
} };

void QCPAbstractItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QCPAbstractItem *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->selectionChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (QCPAbstractItem::*)(bool )>(_a, &QCPAbstractItem::selectionChanged, 0))
            return;
    }
}

const QMetaObject *QCPAbstractItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCPAbstractItem::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15QCPAbstractItemE_t>.strings))
        return static_cast<void*>(this);
    return QCPLayerable::qt_metacast(_clname);
}

int QCPAbstractItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPLayerable::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void QCPAbstractItem::selectionChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}
namespace {
struct qt_meta_tag_ZN19QCPItemStraightLineE_t {};
} // unnamed namespace

template <> constexpr inline auto QCPItemStraightLine::qt_create_metaobjectdata<qt_meta_tag_ZN19QCPItemStraightLineE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QCPItemStraightLine"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QCPItemStraightLine, qt_meta_tag_ZN19QCPItemStraightLineE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QCPItemStraightLine::staticMetaObject = { {
    QMetaObject::SuperData::link<QCPAbstractItem::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19QCPItemStraightLineE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19QCPItemStraightLineE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN19QCPItemStraightLineE_t>.metaTypes,
    nullptr
} };

void QCPItemStraightLine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QCPItemStraightLine *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *QCPItemStraightLine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCPItemStraightLine::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19QCPItemStraightLineE_t>.strings))
        return static_cast<void*>(this);
    return QCPAbstractItem::qt_metacast(_clname);
}

int QCPItemStraightLine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractItem::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN11QCPItemLineE_t {};
} // unnamed namespace

template <> constexpr inline auto QCPItemLine::qt_create_metaobjectdata<qt_meta_tag_ZN11QCPItemLineE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QCPItemLine"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QCPItemLine, qt_meta_tag_ZN11QCPItemLineE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QCPItemLine::staticMetaObject = { {
    QMetaObject::SuperData::link<QCPAbstractItem::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11QCPItemLineE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11QCPItemLineE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11QCPItemLineE_t>.metaTypes,
    nullptr
} };

void QCPItemLine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QCPItemLine *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *QCPItemLine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCPItemLine::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11QCPItemLineE_t>.strings))
        return static_cast<void*>(this);
    return QCPAbstractItem::qt_metacast(_clname);
}

int QCPItemLine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractItem::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN14QCPItemEllipseE_t {};
} // unnamed namespace

template <> constexpr inline auto QCPItemEllipse::qt_create_metaobjectdata<qt_meta_tag_ZN14QCPItemEllipseE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QCPItemEllipse"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QCPItemEllipse, qt_meta_tag_ZN14QCPItemEllipseE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QCPItemEllipse::staticMetaObject = { {
    QMetaObject::SuperData::link<QCPAbstractItem::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14QCPItemEllipseE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14QCPItemEllipseE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14QCPItemEllipseE_t>.metaTypes,
    nullptr
} };

void QCPItemEllipse::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QCPItemEllipse *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *QCPItemEllipse::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCPItemEllipse::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14QCPItemEllipseE_t>.strings))
        return static_cast<void*>(this);
    return QCPAbstractItem::qt_metacast(_clname);
}

int QCPItemEllipse::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractItem::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN11QCPItemRectE_t {};
} // unnamed namespace

template <> constexpr inline auto QCPItemRect::qt_create_metaobjectdata<qt_meta_tag_ZN11QCPItemRectE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QCPItemRect"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QCPItemRect, qt_meta_tag_ZN11QCPItemRectE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QCPItemRect::staticMetaObject = { {
    QMetaObject::SuperData::link<QCPAbstractItem::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11QCPItemRectE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11QCPItemRectE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11QCPItemRectE_t>.metaTypes,
    nullptr
} };

void QCPItemRect::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QCPItemRect *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *QCPItemRect::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCPItemRect::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11QCPItemRectE_t>.strings))
        return static_cast<void*>(this);
    return QCPAbstractItem::qt_metacast(_clname);
}

int QCPItemRect::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractItem::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN13QCPItemPixmapE_t {};
} // unnamed namespace

template <> constexpr inline auto QCPItemPixmap::qt_create_metaobjectdata<qt_meta_tag_ZN13QCPItemPixmapE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QCPItemPixmap"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QCPItemPixmap, qt_meta_tag_ZN13QCPItemPixmapE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QCPItemPixmap::staticMetaObject = { {
    QMetaObject::SuperData::link<QCPAbstractItem::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13QCPItemPixmapE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13QCPItemPixmapE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13QCPItemPixmapE_t>.metaTypes,
    nullptr
} };

void QCPItemPixmap::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QCPItemPixmap *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *QCPItemPixmap::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCPItemPixmap::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13QCPItemPixmapE_t>.strings))
        return static_cast<void*>(this);
    return QCPAbstractItem::qt_metacast(_clname);
}

int QCPItemPixmap::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractItem::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN11QCPItemTextE_t {};
} // unnamed namespace

template <> constexpr inline auto QCPItemText::qt_create_metaobjectdata<qt_meta_tag_ZN11QCPItemTextE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QCPItemText"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QCPItemText, qt_meta_tag_ZN11QCPItemTextE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QCPItemText::staticMetaObject = { {
    QMetaObject::SuperData::link<QCPAbstractItem::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11QCPItemTextE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11QCPItemTextE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11QCPItemTextE_t>.metaTypes,
    nullptr
} };

void QCPItemText::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QCPItemText *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *QCPItemText::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCPItemText::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11QCPItemTextE_t>.strings))
        return static_cast<void*>(this);
    return QCPAbstractItem::qt_metacast(_clname);
}

int QCPItemText::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractItem::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN12QCPItemCurveE_t {};
} // unnamed namespace

template <> constexpr inline auto QCPItemCurve::qt_create_metaobjectdata<qt_meta_tag_ZN12QCPItemCurveE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QCPItemCurve"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QCPItemCurve, qt_meta_tag_ZN12QCPItemCurveE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QCPItemCurve::staticMetaObject = { {
    QMetaObject::SuperData::link<QCPAbstractItem::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12QCPItemCurveE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12QCPItemCurveE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12QCPItemCurveE_t>.metaTypes,
    nullptr
} };

void QCPItemCurve::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QCPItemCurve *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *QCPItemCurve::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCPItemCurve::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12QCPItemCurveE_t>.strings))
        return static_cast<void*>(this);
    return QCPAbstractItem::qt_metacast(_clname);
}

int QCPItemCurve::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractItem::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN14QCPItemBracketE_t {};
} // unnamed namespace

template <> constexpr inline auto QCPItemBracket::qt_create_metaobjectdata<qt_meta_tag_ZN14QCPItemBracketE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QCPItemBracket"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QCPItemBracket, qt_meta_tag_ZN14QCPItemBracketE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QCPItemBracket::staticMetaObject = { {
    QMetaObject::SuperData::link<QCPAbstractItem::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14QCPItemBracketE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14QCPItemBracketE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14QCPItemBracketE_t>.metaTypes,
    nullptr
} };

void QCPItemBracket::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QCPItemBracket *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *QCPItemBracket::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCPItemBracket::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14QCPItemBracketE_t>.strings))
        return static_cast<void*>(this);
    return QCPAbstractItem::qt_metacast(_clname);
}

int QCPItemBracket::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractItem::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN13QCPItemTracerE_t {};
} // unnamed namespace

template <> constexpr inline auto QCPItemTracer::qt_create_metaobjectdata<qt_meta_tag_ZN13QCPItemTracerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QCPItemTracer",
        "TracerStyle",
        "tsNone",
        "tsPlus",
        "tsCrosshair",
        "tsCircle",
        "tsSquare"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'TracerStyle'
        QtMocHelpers::EnumData<enum TracerStyle>(1, 1, QMC::EnumFlags{}).add({
            {    2, TracerStyle::tsNone },
            {    3, TracerStyle::tsPlus },
            {    4, TracerStyle::tsCrosshair },
            {    5, TracerStyle::tsCircle },
            {    6, TracerStyle::tsSquare },
        }),
    };
    return QtMocHelpers::metaObjectData<QCPItemTracer, qt_meta_tag_ZN13QCPItemTracerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QCPItemTracer::staticMetaObject = { {
    QMetaObject::SuperData::link<QCPAbstractItem::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13QCPItemTracerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13QCPItemTracerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13QCPItemTracerE_t>.metaTypes,
    nullptr
} };

void QCPItemTracer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QCPItemTracer *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *QCPItemTracer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCPItemTracer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13QCPItemTracerE_t>.strings))
        return static_cast<void*>(this);
    return QCPAbstractItem::qt_metacast(_clname);
}

int QCPItemTracer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractItem::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN21QCPAbstractLegendItemE_t {};
} // unnamed namespace

template <> constexpr inline auto QCPAbstractLegendItem::qt_create_metaobjectdata<qt_meta_tag_ZN21QCPAbstractLegendItemE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QCPAbstractLegendItem",
        "selectionChanged",
        "",
        "selected"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'selectionChanged'
        QtMocHelpers::SignalData<void(bool)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 3 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QCPAbstractLegendItem, qt_meta_tag_ZN21QCPAbstractLegendItemE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QCPAbstractLegendItem::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN21QCPAbstractLegendItemE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN21QCPAbstractLegendItemE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN21QCPAbstractLegendItemE_t>.metaTypes,
    nullptr
} };

void QCPAbstractLegendItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QCPAbstractLegendItem *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->selectionChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (QCPAbstractLegendItem::*)(bool )>(_a, &QCPAbstractLegendItem::selectionChanged, 0))
            return;
    }
}

const QMetaObject *QCPAbstractLegendItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCPAbstractLegendItem::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN21QCPAbstractLegendItemE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int QCPAbstractLegendItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void QCPAbstractLegendItem::selectionChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}
namespace {
struct qt_meta_tag_ZN22QCPPlottableLegendItemE_t {};
} // unnamed namespace

template <> constexpr inline auto QCPPlottableLegendItem::qt_create_metaobjectdata<qt_meta_tag_ZN22QCPPlottableLegendItemE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QCPPlottableLegendItem"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QCPPlottableLegendItem, qt_meta_tag_ZN22QCPPlottableLegendItemE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QCPPlottableLegendItem::staticMetaObject = { {
    QMetaObject::SuperData::link<QCPAbstractLegendItem::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN22QCPPlottableLegendItemE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN22QCPPlottableLegendItemE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN22QCPPlottableLegendItemE_t>.metaTypes,
    nullptr
} };

void QCPPlottableLegendItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QCPPlottableLegendItem *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *QCPPlottableLegendItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCPPlottableLegendItem::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN22QCPPlottableLegendItemE_t>.strings))
        return static_cast<void*>(this);
    return QCPAbstractLegendItem::qt_metacast(_clname);
}

int QCPPlottableLegendItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractLegendItem::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN9QCPLegendE_t {};
} // unnamed namespace

template <> constexpr inline auto QCPLegend::qt_create_metaobjectdata<qt_meta_tag_ZN9QCPLegendE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QCPLegend",
        "selectionChanged",
        "",
        "QCPLegend::SelectableParts",
        "selection",
        "PositionStyle",
        "psManual",
        "psTopLeft",
        "psTop",
        "psTopRight",
        "psRight",
        "psBottomRight",
        "psBottom",
        "psBottomLeft",
        "psLeft",
        "SelectablePart",
        "spNone",
        "spLegendBox",
        "spItems"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'selectionChanged'
        QtMocHelpers::SignalData<void(QCPLegend::SelectableParts)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'PositionStyle'
        QtMocHelpers::EnumData<enum PositionStyle>(5, 5, QMC::EnumFlags{}).add({
            {    6, PositionStyle::psManual },
            {    7, PositionStyle::psTopLeft },
            {    8, PositionStyle::psTop },
            {    9, PositionStyle::psTopRight },
            {   10, PositionStyle::psRight },
            {   11, PositionStyle::psBottomRight },
            {   12, PositionStyle::psBottom },
            {   13, PositionStyle::psBottomLeft },
            {   14, PositionStyle::psLeft },
        }),
        // enum 'SelectablePart'
        QtMocHelpers::EnumData<enum SelectablePart>(15, 15, QMC::EnumFlags{}).add({
            {   16, SelectablePart::spNone },
            {   17, SelectablePart::spLegendBox },
            {   18, SelectablePart::spItems },
        }),
    };
    return QtMocHelpers::metaObjectData<QCPLegend, qt_meta_tag_ZN9QCPLegendE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QCPLegend::staticMetaObject = { {
    QMetaObject::SuperData::link<QCPLayerable::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9QCPLegendE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9QCPLegendE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9QCPLegendE_t>.metaTypes,
    nullptr
} };

void QCPLegend::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QCPLegend *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->selectionChanged((*reinterpret_cast<std::add_pointer_t<QCPLegend::SelectableParts>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (QCPLegend::*)(QCPLegend::SelectableParts )>(_a, &QCPLegend::selectionChanged, 0))
            return;
    }
}

const QMetaObject *QCPLegend::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCPLegend::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9QCPLegendE_t>.strings))
        return static_cast<void*>(this);
    return QCPLayerable::qt_metacast(_clname);
}

int QCPLegend::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPLayerable::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void QCPLegend::selectionChanged(QCPLegend::SelectableParts _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}
namespace {
struct qt_meta_tag_ZN7QCPGridE_t {};
} // unnamed namespace

template <> constexpr inline auto QCPGrid::qt_create_metaobjectdata<qt_meta_tag_ZN7QCPGridE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QCPGrid"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QCPGrid, qt_meta_tag_ZN7QCPGridE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QCPGrid::staticMetaObject = { {
    QMetaObject::SuperData::link<QCPLayerable::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7QCPGridE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7QCPGridE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN7QCPGridE_t>.metaTypes,
    nullptr
} };

void QCPGrid::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QCPGrid *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *QCPGrid::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCPGrid::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7QCPGridE_t>.strings))
        return static_cast<void*>(this);
    return QCPLayerable::qt_metacast(_clname);
}

int QCPGrid::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPLayerable::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN7QCPAxisE_t {};
} // unnamed namespace

template <> constexpr inline auto QCPAxis::qt_create_metaobjectdata<qt_meta_tag_ZN7QCPAxisE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QCPAxis",
        "ticksRequest",
        "",
        "rangeChanged",
        "QCPRange",
        "newRange",
        "selectionChanged",
        "QCPAxis::SelectableParts",
        "selection",
        "setRange",
        "range",
        "setSelectable",
        "selectable",
        "setSelected",
        "selected",
        "axisType",
        "AxisType",
        "scaleType",
        "ScaleType",
        "scaleLogBase",
        "axisRect",
        "QRect",
        "grid",
        "subGrid",
        "autoTicks",
        "autoTickCount",
        "autoTickLabels",
        "autoTickStep",
        "autoSubTicks",
        "ticks",
        "tickLabels",
        "tickLabelPadding",
        "tickLabelType",
        "LabelType",
        "tickLabelFont",
        "QFont",
        "tickLabelRotation",
        "dateTimeFormat",
        "numberFormat",
        "tickStep",
        "tickVector",
        "QList<double>",
        "tickVectorLabels",
        "subTickCount",
        "basePen",
        "QPen",
        "gridPen",
        "subGridPen",
        "tickPen",
        "subTickPen",
        "labelFont",
        "label",
        "labelPadding",
        "atLeft",
        "atRight",
        "atTop",
        "atBottom",
        "ltNumber",
        "ltDateTime",
        "stLinear",
        "stLogarithmic",
        "SelectablePart",
        "spNone",
        "spAxis",
        "spTickLabels",
        "spAxisLabel"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'ticksRequest'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'rangeChanged'
        QtMocHelpers::SignalData<void(const QCPRange &)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 4, 5 },
        }}),
        // Signal 'selectionChanged'
        QtMocHelpers::SignalData<void(QCPAxis::SelectableParts)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 7, 8 },
        }}),
        // Slot 'setRange'
        QtMocHelpers::SlotData<void(const QCPRange &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 4, 10 },
        }}),
        // Slot 'setSelectable'
        QtMocHelpers::SlotData<void(const QCPAxis::SelectableParts &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 7, 12 },
        }}),
        // Slot 'setSelected'
        QtMocHelpers::SlotData<void(const QCPAxis::SelectableParts &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 7, 14 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'axisType'
        QtMocHelpers::PropertyData<enum AxisType>(15, 0x80000000 | 16, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'scaleType'
        QtMocHelpers::PropertyData<enum ScaleType>(17, 0x80000000 | 18, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'scaleLogBase'
        QtMocHelpers::PropertyData<double>(19, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'axisRect'
        QtMocHelpers::PropertyData<QRect>(20, 0x80000000 | 21, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'range'
        QtMocHelpers::PropertyData<QCPRange>(10, 0x80000000 | 4, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'grid'
        QtMocHelpers::PropertyData<bool>(22, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'subGrid'
        QtMocHelpers::PropertyData<bool>(23, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'autoTicks'
        QtMocHelpers::PropertyData<bool>(24, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'autoTickCount'
        QtMocHelpers::PropertyData<int>(25, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'autoTickLabels'
        QtMocHelpers::PropertyData<bool>(26, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'autoTickStep'
        QtMocHelpers::PropertyData<bool>(27, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'autoSubTicks'
        QtMocHelpers::PropertyData<bool>(28, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'ticks'
        QtMocHelpers::PropertyData<bool>(29, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'tickLabels'
        QtMocHelpers::PropertyData<bool>(30, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'tickLabelPadding'
        QtMocHelpers::PropertyData<int>(31, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'tickLabelType'
        QtMocHelpers::PropertyData<enum LabelType>(32, 0x80000000 | 33, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'tickLabelFont'
        QtMocHelpers::PropertyData<QFont>(34, 0x80000000 | 35, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'tickLabelRotation'
        QtMocHelpers::PropertyData<double>(36, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'dateTimeFormat'
        QtMocHelpers::PropertyData<QString>(37, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'numberFormat'
        QtMocHelpers::PropertyData<QString>(38, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'tickStep'
        QtMocHelpers::PropertyData<double>(39, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'tickVector'
        QtMocHelpers::PropertyData<QList<double>>(40, 0x80000000 | 41, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'tickVectorLabels'
        QtMocHelpers::PropertyData<QList<QString>>(42, QMetaType::QStringList, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'subTickCount'
        QtMocHelpers::PropertyData<int>(43, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'basePen'
        QtMocHelpers::PropertyData<QPen>(44, 0x80000000 | 45, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'gridPen'
        QtMocHelpers::PropertyData<QPen>(46, 0x80000000 | 45, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'subGridPen'
        QtMocHelpers::PropertyData<QPen>(47, 0x80000000 | 45, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'tickPen'
        QtMocHelpers::PropertyData<QPen>(48, 0x80000000 | 45, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'subTickPen'
        QtMocHelpers::PropertyData<QPen>(49, 0x80000000 | 45, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'labelFont'
        QtMocHelpers::PropertyData<QFont>(50, 0x80000000 | 35, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'label'
        QtMocHelpers::PropertyData<QString>(51, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'labelPadding'
        QtMocHelpers::PropertyData<int>(52, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'AxisType'
        QtMocHelpers::EnumData<enum AxisType>(16, 16, QMC::EnumFlags{}).add({
            {   53, AxisType::atLeft },
            {   54, AxisType::atRight },
            {   55, AxisType::atTop },
            {   56, AxisType::atBottom },
        }),
        // enum 'LabelType'
        QtMocHelpers::EnumData<enum LabelType>(33, 33, QMC::EnumFlags{}).add({
            {   57, LabelType::ltNumber },
            {   58, LabelType::ltDateTime },
        }),
        // enum 'ScaleType'
        QtMocHelpers::EnumData<enum ScaleType>(18, 18, QMC::EnumFlags{}).add({
            {   59, ScaleType::stLinear },
            {   60, ScaleType::stLogarithmic },
        }),
        // enum 'SelectablePart'
        QtMocHelpers::EnumData<enum SelectablePart>(61, 61, QMC::EnumFlags{}).add({
            {   62, SelectablePart::spNone },
            {   63, SelectablePart::spAxis },
            {   64, SelectablePart::spTickLabels },
            {   65, SelectablePart::spAxisLabel },
        }),
    };
    return QtMocHelpers::metaObjectData<QCPAxis, qt_meta_tag_ZN7QCPAxisE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QCPAxis::staticMetaObject = { {
    QMetaObject::SuperData::link<QCPLayerable::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7QCPAxisE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7QCPAxisE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN7QCPAxisE_t>.metaTypes,
    nullptr
} };

void QCPAxis::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QCPAxis *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->ticksRequest(); break;
        case 1: _t->rangeChanged((*reinterpret_cast<std::add_pointer_t<QCPRange>>(_a[1]))); break;
        case 2: _t->selectionChanged((*reinterpret_cast<std::add_pointer_t<QCPAxis::SelectableParts>>(_a[1]))); break;
        case 3: _t->setRange((*reinterpret_cast<std::add_pointer_t<QCPRange>>(_a[1]))); break;
        case 4: _t->setSelectable((*reinterpret_cast<std::add_pointer_t<QCPAxis::SelectableParts>>(_a[1]))); break;
        case 5: _t->setSelected((*reinterpret_cast<std::add_pointer_t<QCPAxis::SelectableParts>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (QCPAxis::*)()>(_a, &QCPAxis::ticksRequest, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (QCPAxis::*)(const QCPRange & )>(_a, &QCPAxis::rangeChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (QCPAxis::*)(QCPAxis::SelectableParts )>(_a, &QCPAxis::selectionChanged, 2))
            return;
    }
    if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 21:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<double> >(); break;
        }
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<enum AxisType*>(_v) = _t->axisType(); break;
        case 1: *reinterpret_cast<enum ScaleType*>(_v) = _t->scaleType(); break;
        case 2: *reinterpret_cast<double*>(_v) = _t->scaleLogBase(); break;
        case 3: *reinterpret_cast<QRect*>(_v) = _t->axisRect(); break;
        case 4: *reinterpret_cast<QCPRange*>(_v) = _t->range(); break;
        case 5: *reinterpret_cast<bool*>(_v) = _t->grid(); break;
        case 6: *reinterpret_cast<bool*>(_v) = _t->subGrid(); break;
        case 7: *reinterpret_cast<bool*>(_v) = _t->autoTicks(); break;
        case 8: *reinterpret_cast<int*>(_v) = _t->autoTickCount(); break;
        case 9: *reinterpret_cast<bool*>(_v) = _t->autoTickLabels(); break;
        case 10: *reinterpret_cast<bool*>(_v) = _t->autoTickStep(); break;
        case 11: *reinterpret_cast<bool*>(_v) = _t->autoSubTicks(); break;
        case 12: *reinterpret_cast<bool*>(_v) = _t->ticks(); break;
        case 13: *reinterpret_cast<bool*>(_v) = _t->tickLabels(); break;
        case 14: *reinterpret_cast<int*>(_v) = _t->tickLabelPadding(); break;
        case 15: *reinterpret_cast<enum LabelType*>(_v) = _t->tickLabelType(); break;
        case 16: *reinterpret_cast<QFont*>(_v) = _t->tickLabelFont(); break;
        case 17: *reinterpret_cast<double*>(_v) = _t->tickLabelRotation(); break;
        case 18: *reinterpret_cast<QString*>(_v) = _t->dateTimeFormat(); break;
        case 19: *reinterpret_cast<QString*>(_v) = _t->numberFormat(); break;
        case 20: *reinterpret_cast<double*>(_v) = _t->tickStep(); break;
        case 21: *reinterpret_cast<QList<double>*>(_v) = _t->tickVector(); break;
        case 22: *reinterpret_cast<QList<QString>*>(_v) = _t->tickVectorLabels(); break;
        case 23: *reinterpret_cast<int*>(_v) = _t->subTickCount(); break;
        case 24: *reinterpret_cast<QPen*>(_v) = _t->basePen(); break;
        case 25: *reinterpret_cast<QPen*>(_v) = _t->gridPen(); break;
        case 26: *reinterpret_cast<QPen*>(_v) = _t->subGridPen(); break;
        case 27: *reinterpret_cast<QPen*>(_v) = _t->tickPen(); break;
        case 28: *reinterpret_cast<QPen*>(_v) = _t->subTickPen(); break;
        case 29: *reinterpret_cast<QFont*>(_v) = _t->labelFont(); break;
        case 30: *reinterpret_cast<QString*>(_v) = _t->label(); break;
        case 31: *reinterpret_cast<int*>(_v) = _t->labelPadding(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setAxisType(*reinterpret_cast<enum AxisType*>(_v)); break;
        case 1: _t->setScaleType(*reinterpret_cast<enum ScaleType*>(_v)); break;
        case 2: _t->setScaleLogBase(*reinterpret_cast<double*>(_v)); break;
        case 3: _t->setAxisRect(*reinterpret_cast<QRect*>(_v)); break;
        case 4: _t->setRange(*reinterpret_cast<QCPRange*>(_v)); break;
        case 5: _t->setGrid(*reinterpret_cast<bool*>(_v)); break;
        case 6: _t->setSubGrid(*reinterpret_cast<bool*>(_v)); break;
        case 7: _t->setAutoTicks(*reinterpret_cast<bool*>(_v)); break;
        case 8: _t->setAutoTickCount(*reinterpret_cast<int*>(_v)); break;
        case 9: _t->setAutoTickLabels(*reinterpret_cast<bool*>(_v)); break;
        case 10: _t->setAutoTickStep(*reinterpret_cast<bool*>(_v)); break;
        case 11: _t->setAutoSubTicks(*reinterpret_cast<bool*>(_v)); break;
        case 12: _t->setTicks(*reinterpret_cast<bool*>(_v)); break;
        case 13: _t->setTickLabels(*reinterpret_cast<bool*>(_v)); break;
        case 14: _t->setTickLabelPadding(*reinterpret_cast<int*>(_v)); break;
        case 15: _t->setTickLabelType(*reinterpret_cast<enum LabelType*>(_v)); break;
        case 16: _t->setTickLabelFont(*reinterpret_cast<QFont*>(_v)); break;
        case 17: _t->setTickLabelRotation(*reinterpret_cast<double*>(_v)); break;
        case 18: _t->setDateTimeFormat(*reinterpret_cast<QString*>(_v)); break;
        case 19: _t->setNumberFormat(*reinterpret_cast<QString*>(_v)); break;
        case 20: _t->setTickStep(*reinterpret_cast<double*>(_v)); break;
        case 21: _t->setTickVector(*reinterpret_cast<QList<double>*>(_v)); break;
        case 22: _t->setTickVectorLabels(*reinterpret_cast<QList<QString>*>(_v)); break;
        case 23: _t->setSubTickCount(*reinterpret_cast<int*>(_v)); break;
        case 24: _t->setBasePen(*reinterpret_cast<QPen*>(_v)); break;
        case 25: _t->setGridPen(*reinterpret_cast<QPen*>(_v)); break;
        case 26: _t->setSubGridPen(*reinterpret_cast<QPen*>(_v)); break;
        case 27: _t->setTickPen(*reinterpret_cast<QPen*>(_v)); break;
        case 28: _t->setSubTickPen(*reinterpret_cast<QPen*>(_v)); break;
        case 29: _t->setLabelFont(*reinterpret_cast<QFont*>(_v)); break;
        case 30: _t->setLabel(*reinterpret_cast<QString*>(_v)); break;
        case 31: _t->setLabelPadding(*reinterpret_cast<int*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *QCPAxis::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCPAxis::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7QCPAxisE_t>.strings))
        return static_cast<void*>(this);
    return QCPLayerable::qt_metacast(_clname);
}

int QCPAxis::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPLayerable::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 32;
    }
    return _id;
}

// SIGNAL 0
void QCPAxis::ticksRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void QCPAxis::rangeChanged(const QCPRange & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void QCPAxis::selectionChanged(QCPAxis::SelectableParts _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}
namespace {
struct qt_meta_tag_ZN11QCustomPlotE_t {};
} // unnamed namespace

template <> constexpr inline auto QCustomPlot::qt_create_metaobjectdata<qt_meta_tag_ZN11QCustomPlotE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QCustomPlot",
        "mouseDoubleClick",
        "",
        "QMouseEvent*",
        "event",
        "mousePress",
        "mouseMove",
        "mouseRelease",
        "mouseWheel",
        "QWheelEvent*",
        "plottableClick",
        "QCPAbstractPlottable*",
        "plottable",
        "plottableDoubleClick",
        "itemClick",
        "QCPAbstractItem*",
        "item",
        "itemDoubleClick",
        "axisClick",
        "QCPAxis*",
        "axis",
        "QCPAxis::SelectablePart",
        "part",
        "axisDoubleClick",
        "legendClick",
        "QCPLegend*",
        "legend",
        "QCPAbstractLegendItem*",
        "legendDoubleClick",
        "titleClick",
        "titleDoubleClick",
        "selectionChangedByUser",
        "beforeReplot",
        "afterReplot",
        "deselectAll",
        "replot",
        "rescaleAxes",
        "title",
        "axisRect",
        "QRect",
        "marginLeft",
        "marginRight",
        "marginTop",
        "marginBottom",
        "autoMargin",
        "color",
        "QColor",
        "rangeDrag",
        "Qt::Orientations",
        "rangeZoom",
        "Interaction",
        "iRangeDrag",
        "iRangeZoom",
        "iMultiSelect",
        "iSelectTitle",
        "iSelectPlottables",
        "iSelectAxes",
        "iSelectLegend",
        "iSelectItems",
        "LayerInsertMode",
        "limBelow",
        "limAbove"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'mouseDoubleClick'
        QtMocHelpers::SignalData<void(QMouseEvent *)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'mousePress'
        QtMocHelpers::SignalData<void(QMouseEvent *)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'mouseMove'
        QtMocHelpers::SignalData<void(QMouseEvent *)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'mouseRelease'
        QtMocHelpers::SignalData<void(QMouseEvent *)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'mouseWheel'
        QtMocHelpers::SignalData<void(QWheelEvent *)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 4 },
        }}),
        // Signal 'plottableClick'
        QtMocHelpers::SignalData<void(QCPAbstractPlottable *, QMouseEvent *)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 11, 12 }, { 0x80000000 | 3, 4 },
        }}),
        // Signal 'plottableDoubleClick'
        QtMocHelpers::SignalData<void(QCPAbstractPlottable *, QMouseEvent *)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 11, 12 }, { 0x80000000 | 3, 4 },
        }}),
        // Signal 'itemClick'
        QtMocHelpers::SignalData<void(QCPAbstractItem *, QMouseEvent *)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 15, 16 }, { 0x80000000 | 3, 4 },
        }}),
        // Signal 'itemDoubleClick'
        QtMocHelpers::SignalData<void(QCPAbstractItem *, QMouseEvent *)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 15, 16 }, { 0x80000000 | 3, 4 },
        }}),
        // Signal 'axisClick'
        QtMocHelpers::SignalData<void(QCPAxis *, QCPAxis::SelectablePart, QMouseEvent *)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 19, 20 }, { 0x80000000 | 21, 22 }, { 0x80000000 | 3, 4 },
        }}),
        // Signal 'axisDoubleClick'
        QtMocHelpers::SignalData<void(QCPAxis *, QCPAxis::SelectablePart, QMouseEvent *)>(23, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 19, 20 }, { 0x80000000 | 21, 22 }, { 0x80000000 | 3, 4 },
        }}),
        // Signal 'legendClick'
        QtMocHelpers::SignalData<void(QCPLegend *, QCPAbstractLegendItem *, QMouseEvent *)>(24, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 25, 26 }, { 0x80000000 | 27, 16 }, { 0x80000000 | 3, 4 },
        }}),
        // Signal 'legendDoubleClick'
        QtMocHelpers::SignalData<void(QCPLegend *, QCPAbstractLegendItem *, QMouseEvent *)>(28, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 25, 26 }, { 0x80000000 | 27, 16 }, { 0x80000000 | 3, 4 },
        }}),
        // Signal 'titleClick'
        QtMocHelpers::SignalData<void(QMouseEvent *)>(29, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'titleDoubleClick'
        QtMocHelpers::SignalData<void(QMouseEvent *)>(30, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'selectionChangedByUser'
        QtMocHelpers::SignalData<void()>(31, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'beforeReplot'
        QtMocHelpers::SignalData<void()>(32, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'afterReplot'
        QtMocHelpers::SignalData<void()>(33, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'deselectAll'
        QtMocHelpers::SlotData<void()>(34, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'replot'
        QtMocHelpers::SlotData<void()>(35, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'rescaleAxes'
        QtMocHelpers::SlotData<void()>(36, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'title'
        QtMocHelpers::PropertyData<QString>(37, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'axisRect'
        QtMocHelpers::PropertyData<QRect>(38, 0x80000000 | 39, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'marginLeft'
        QtMocHelpers::PropertyData<int>(40, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'marginRight'
        QtMocHelpers::PropertyData<int>(41, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'marginTop'
        QtMocHelpers::PropertyData<int>(42, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'marginBottom'
        QtMocHelpers::PropertyData<int>(43, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'autoMargin'
        QtMocHelpers::PropertyData<int>(44, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'color'
        QtMocHelpers::PropertyData<QColor>(45, 0x80000000 | 46, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'rangeDrag'
        QtMocHelpers::PropertyData<Qt::Orientations>(47, 0x80000000 | 48, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'rangeZoom'
        QtMocHelpers::PropertyData<Qt::Orientations>(49, 0x80000000 | 48, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'Interaction'
        QtMocHelpers::EnumData<enum Interaction>(50, 50, QMC::EnumFlags{}).add({
            {   51, Interaction::iRangeDrag },
            {   52, Interaction::iRangeZoom },
            {   53, Interaction::iMultiSelect },
            {   54, Interaction::iSelectTitle },
            {   55, Interaction::iSelectPlottables },
            {   56, Interaction::iSelectAxes },
            {   57, Interaction::iSelectLegend },
            {   58, Interaction::iSelectItems },
        }),
        // enum 'LayerInsertMode'
        QtMocHelpers::EnumData<enum LayerInsertMode>(59, 59, QMC::EnumFlags{}).add({
            {   60, LayerInsertMode::limBelow },
            {   61, LayerInsertMode::limAbove },
        }),
    };
    return QtMocHelpers::metaObjectData<QCustomPlot, qt_meta_tag_ZN11QCustomPlotE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QCustomPlot::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11QCustomPlotE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11QCustomPlotE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11QCustomPlotE_t>.metaTypes,
    nullptr
} };

void QCustomPlot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QCustomPlot *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->mouseDoubleClick((*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[1]))); break;
        case 1: _t->mousePress((*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[1]))); break;
        case 2: _t->mouseMove((*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[1]))); break;
        case 3: _t->mouseRelease((*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[1]))); break;
        case 4: _t->mouseWheel((*reinterpret_cast<std::add_pointer_t<QWheelEvent*>>(_a[1]))); break;
        case 5: _t->plottableClick((*reinterpret_cast<std::add_pointer_t<QCPAbstractPlottable*>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[2]))); break;
        case 6: _t->plottableDoubleClick((*reinterpret_cast<std::add_pointer_t<QCPAbstractPlottable*>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[2]))); break;
        case 7: _t->itemClick((*reinterpret_cast<std::add_pointer_t<QCPAbstractItem*>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[2]))); break;
        case 8: _t->itemDoubleClick((*reinterpret_cast<std::add_pointer_t<QCPAbstractItem*>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[2]))); break;
        case 9: _t->axisClick((*reinterpret_cast<std::add_pointer_t<QCPAxis*>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QCPAxis::SelectablePart>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[3]))); break;
        case 10: _t->axisDoubleClick((*reinterpret_cast<std::add_pointer_t<QCPAxis*>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QCPAxis::SelectablePart>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[3]))); break;
        case 11: _t->legendClick((*reinterpret_cast<std::add_pointer_t<QCPLegend*>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QCPAbstractLegendItem*>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[3]))); break;
        case 12: _t->legendDoubleClick((*reinterpret_cast<std::add_pointer_t<QCPLegend*>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QCPAbstractLegendItem*>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[3]))); break;
        case 13: _t->titleClick((*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[1]))); break;
        case 14: _t->titleDoubleClick((*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[1]))); break;
        case 15: _t->selectionChangedByUser(); break;
        case 16: _t->beforeReplot(); break;
        case 17: _t->afterReplot(); break;
        case 18: _t->deselectAll(); break;
        case 19: _t->replot(); break;
        case 20: _t->rescaleAxes(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QCPAbstractPlottable* >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QCPAbstractPlottable* >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QCPAbstractItem* >(); break;
            }
            break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QCPAbstractItem* >(); break;
            }
            break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QCPAxis* >(); break;
            }
            break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QCPAxis* >(); break;
            }
            break;
        case 11:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QCPAbstractLegendItem* >(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QCPLegend* >(); break;
            }
            break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QCPAbstractLegendItem* >(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QCPLegend* >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (QCustomPlot::*)(QMouseEvent * )>(_a, &QCustomPlot::mouseDoubleClick, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (QCustomPlot::*)(QMouseEvent * )>(_a, &QCustomPlot::mousePress, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (QCustomPlot::*)(QMouseEvent * )>(_a, &QCustomPlot::mouseMove, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (QCustomPlot::*)(QMouseEvent * )>(_a, &QCustomPlot::mouseRelease, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (QCustomPlot::*)(QWheelEvent * )>(_a, &QCustomPlot::mouseWheel, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (QCustomPlot::*)(QCPAbstractPlottable * , QMouseEvent * )>(_a, &QCustomPlot::plottableClick, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (QCustomPlot::*)(QCPAbstractPlottable * , QMouseEvent * )>(_a, &QCustomPlot::plottableDoubleClick, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (QCustomPlot::*)(QCPAbstractItem * , QMouseEvent * )>(_a, &QCustomPlot::itemClick, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (QCustomPlot::*)(QCPAbstractItem * , QMouseEvent * )>(_a, &QCustomPlot::itemDoubleClick, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (QCustomPlot::*)(QCPAxis * , QCPAxis::SelectablePart , QMouseEvent * )>(_a, &QCustomPlot::axisClick, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (QCustomPlot::*)(QCPAxis * , QCPAxis::SelectablePart , QMouseEvent * )>(_a, &QCustomPlot::axisDoubleClick, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (QCustomPlot::*)(QCPLegend * , QCPAbstractLegendItem * , QMouseEvent * )>(_a, &QCustomPlot::legendClick, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (QCustomPlot::*)(QCPLegend * , QCPAbstractLegendItem * , QMouseEvent * )>(_a, &QCustomPlot::legendDoubleClick, 12))
            return;
        if (QtMocHelpers::indexOfMethod<void (QCustomPlot::*)(QMouseEvent * )>(_a, &QCustomPlot::titleClick, 13))
            return;
        if (QtMocHelpers::indexOfMethod<void (QCustomPlot::*)(QMouseEvent * )>(_a, &QCustomPlot::titleDoubleClick, 14))
            return;
        if (QtMocHelpers::indexOfMethod<void (QCustomPlot::*)()>(_a, &QCustomPlot::selectionChangedByUser, 15))
            return;
        if (QtMocHelpers::indexOfMethod<void (QCustomPlot::*)()>(_a, &QCustomPlot::beforeReplot, 16))
            return;
        if (QtMocHelpers::indexOfMethod<void (QCustomPlot::*)()>(_a, &QCustomPlot::afterReplot, 17))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QString*>(_v) = _t->title(); break;
        case 1: *reinterpret_cast<QRect*>(_v) = _t->axisRect(); break;
        case 2: *reinterpret_cast<int*>(_v) = _t->marginLeft(); break;
        case 3: *reinterpret_cast<int*>(_v) = _t->marginRight(); break;
        case 4: *reinterpret_cast<int*>(_v) = _t->marginTop(); break;
        case 5: *reinterpret_cast<int*>(_v) = _t->marginBottom(); break;
        case 6: *reinterpret_cast<int*>(_v) = _t->autoMargin(); break;
        case 7: *reinterpret_cast<QColor*>(_v) = _t->color(); break;
        case 8: *reinterpret_cast<Qt::Orientations*>(_v) = _t->rangeDrag(); break;
        case 9: *reinterpret_cast<Qt::Orientations*>(_v) = _t->rangeZoom(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setTitle(*reinterpret_cast<QString*>(_v)); break;
        case 1: _t->setAxisRect(*reinterpret_cast<QRect*>(_v)); break;
        case 2: _t->setMarginLeft(*reinterpret_cast<int*>(_v)); break;
        case 3: _t->setMarginRight(*reinterpret_cast<int*>(_v)); break;
        case 4: _t->setMarginTop(*reinterpret_cast<int*>(_v)); break;
        case 5: _t->setMarginBottom(*reinterpret_cast<int*>(_v)); break;
        case 6: _t->setAutoMargin(*reinterpret_cast<int*>(_v)); break;
        case 7: _t->setColor(*reinterpret_cast<QColor*>(_v)); break;
        case 8: _t->setRangeDrag(*reinterpret_cast<Qt::Orientations*>(_v)); break;
        case 9: _t->setRangeZoom(*reinterpret_cast<Qt::Orientations*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *QCustomPlot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCustomPlot::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11QCustomPlotE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int QCustomPlot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void QCustomPlot::mouseDoubleClick(QMouseEvent * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void QCustomPlot::mousePress(QMouseEvent * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void QCustomPlot::mouseMove(QMouseEvent * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void QCustomPlot::mouseRelease(QMouseEvent * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void QCustomPlot::mouseWheel(QWheelEvent * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void QCustomPlot::plottableClick(QCPAbstractPlottable * _t1, QMouseEvent * _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2);
}

// SIGNAL 6
void QCustomPlot::plottableDoubleClick(QCPAbstractPlottable * _t1, QMouseEvent * _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2);
}

// SIGNAL 7
void QCustomPlot::itemClick(QCPAbstractItem * _t1, QMouseEvent * _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1, _t2);
}

// SIGNAL 8
void QCustomPlot::itemDoubleClick(QCPAbstractItem * _t1, QMouseEvent * _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1, _t2);
}

// SIGNAL 9
void QCustomPlot::axisClick(QCPAxis * _t1, QCPAxis::SelectablePart _t2, QMouseEvent * _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1, _t2, _t3);
}

// SIGNAL 10
void QCustomPlot::axisDoubleClick(QCPAxis * _t1, QCPAxis::SelectablePart _t2, QMouseEvent * _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1, _t2, _t3);
}

// SIGNAL 11
void QCustomPlot::legendClick(QCPLegend * _t1, QCPAbstractLegendItem * _t2, QMouseEvent * _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 11, nullptr, _t1, _t2, _t3);
}

// SIGNAL 12
void QCustomPlot::legendDoubleClick(QCPLegend * _t1, QCPAbstractLegendItem * _t2, QMouseEvent * _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 12, nullptr, _t1, _t2, _t3);
}

// SIGNAL 13
void QCustomPlot::titleClick(QMouseEvent * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 13, nullptr, _t1);
}

// SIGNAL 14
void QCustomPlot::titleDoubleClick(QMouseEvent * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 14, nullptr, _t1);
}

// SIGNAL 15
void QCustomPlot::selectionChangedByUser()
{
    QMetaObject::activate(this, &staticMetaObject, 15, nullptr);
}

// SIGNAL 16
void QCustomPlot::beforeReplot()
{
    QMetaObject::activate(this, &staticMetaObject, 16, nullptr);
}

// SIGNAL 17
void QCustomPlot::afterReplot()
{
    QMetaObject::activate(this, &staticMetaObject, 17, nullptr);
}
QT_WARNING_POP
