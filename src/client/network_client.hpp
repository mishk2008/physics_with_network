#pragma once

#include "types.hpp"
#include "itransport.hpp"
#include "serialize.hpp"

#include <QWidget>
#include <QTcpSocket>
#include <QByteArray>

class TcpNetworkClient : public IClientTransport {
    Q_OBJECT
public:
    explicit TcpNetworkClient(QObject* parent = nullptr);
    void connect_to_server(const QString& host, quint16 port) override;
    void disconnect_from_server() override;
private slots:
    void on_ready_read();

private:
    QTcpSocket* socket_;
    QByteArray network_buffer_;
};
