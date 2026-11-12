#include "network_client.hpp"

TcpNetworkClient::TcpNetworkClient(QObject *parent)
: IClientTransport(parent) {

    socket_=new QTcpSocket(this);
    connect(socket_, &QTcpSocket::readyRead, this, &TcpNetworkClient::on_ready_read);
}

void TcpNetworkClient::on_ready_read() {
    network_buffer_.append(socket_->readAll());

    while (network_buffer_.size() >= 4) {
        uint32_t len;

        std::memcpy(&len, network_buffer_.constData(), 4);
        to_network(len);

        if (network_buffer_.size() < 4+len) break;

        std::vector<uint8_t> payload(len);
        std::memcpy(payload.data(), network_buffer_.constData()+4, len);

        network_buffer_.remove(0, 4+len);

        Snapshot snap{Marshaling::deserialize(payload.data(), payload.size())};

        emit snapshot_received(snap);
    }
}

void TcpNetworkClient::connect_to_server(const QString& host, quint16 port) {
    socket_->connectToHost(host, port);
    emit connected();
}

void TcpNetworkClient::disconnect_from_server() {
    socket_->disconnectFromHost();
    emit disconnected();
}

#include "moc_itransport.cpp" /* 
чтобы нормально инклудало библиотеку itransport.hpp, что позволяет не создавать пустой костыльный .cpp файл;
вклеивает метаобъекты библиотеки (сигналы в данном случае) в конец network_client.cpp;
*/