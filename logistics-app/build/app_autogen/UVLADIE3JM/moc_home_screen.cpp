/****************************************************************************
** Meta object code from reading C++ file 'home_screen.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/home_screen.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'home_screen.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.2. It"
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
struct qt_meta_tag_ZN10HomeScreenE_t {};
} // unnamed namespace

template <> constexpr inline auto HomeScreen::qt_create_metaobjectdata<qt_meta_tag_ZN10HomeScreenE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "HomeScreen",
        "createOrderRequested",
        "",
        "deleteOrderRequested",
        "orderId",
        "detailsRequested",
        "editOrderRequested"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'createOrderRequested'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'deleteOrderRequested'
        QtMocHelpers::SignalData<void(long long)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::LongLong, 4 },
        }}),
        // Signal 'detailsRequested'
        QtMocHelpers::SignalData<void(long long)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::LongLong, 4 },
        }}),
        // Signal 'editOrderRequested'
        QtMocHelpers::SignalData<void(long long)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::LongLong, 4 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<HomeScreen, qt_meta_tag_ZN10HomeScreenE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject HomeScreen::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10HomeScreenE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10HomeScreenE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10HomeScreenE_t>.metaTypes,
    nullptr
} };

void HomeScreen::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<HomeScreen *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->createOrderRequested(); break;
        case 1: _t->deleteOrderRequested((*reinterpret_cast<std::add_pointer_t<qlonglong>>(_a[1]))); break;
        case 2: _t->detailsRequested((*reinterpret_cast<std::add_pointer_t<qlonglong>>(_a[1]))); break;
        case 3: _t->editOrderRequested((*reinterpret_cast<std::add_pointer_t<qlonglong>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (HomeScreen::*)()>(_a, &HomeScreen::createOrderRequested, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (HomeScreen::*)(long long )>(_a, &HomeScreen::deleteOrderRequested, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (HomeScreen::*)(long long )>(_a, &HomeScreen::detailsRequested, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (HomeScreen::*)(long long )>(_a, &HomeScreen::editOrderRequested, 3))
            return;
    }
}

const QMetaObject *HomeScreen::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HomeScreen::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10HomeScreenE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int HomeScreen::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void HomeScreen::createOrderRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void HomeScreen::deleteOrderRequested(long long _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void HomeScreen::detailsRequested(long long _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void HomeScreen::editOrderRequested(long long _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}
QT_WARNING_POP
