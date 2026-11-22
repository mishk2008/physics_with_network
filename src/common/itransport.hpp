#pragma once

#include "types.hpp"

#include <memory>

class IServerTransport{
public:
    virtual ~IServerTransport() = default;
    virtual void start() = 0;
    virtual void broadcast_snapshot(const Snapshot& snapshot) = 0;
    virtual void remove_session(std::shared_ptr<class Session> session) = 0;    
};

#ifdef QT_CORE_LIB // макрос чтобы сервер оставался чистым C++ без Qt

#include <QObject>
#include <QString>

class IClientTransport : public QObject {
    Q_OBJECT
public:
    virtual ~IClientTransport() = default;
    explicit IClientTransport(QObject* parent=nullptr) : QObject(parent) {}
    virtual void connect_to_server(const QString& host, quint16 port) = 0;
    virtual void disconnect_from_server() = 0;

signals:
    void snapshot_received(const Snapshot& snap);
    void connected();
    void disconnected();
    void error_occured(const QString& error_message);
};
#endif
