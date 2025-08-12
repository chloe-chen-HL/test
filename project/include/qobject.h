#pragma once

class QEvent;

class QObject {
public:
    virtual ~QObject() = default;

protected:
    virtual bool event(QEvent* /*e*/);
};

inline bool QObject::event(QEvent*) { return false; }

class QDebugStream {
public:
    template <typename T>
    QDebugStream& operator<<(const T&) { return *this; }
};

inline QDebugStream qDebug() { return QDebugStream(); }