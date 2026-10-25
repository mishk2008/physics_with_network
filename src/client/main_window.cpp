#include <QPainter>

#include "main_window.hpp"
#include "serialize.hpp"

MainWindow::MainWindow(QWidget *parent)
: QWidget(parent) {
    setWindowTitle("Moving balls");
    resize(800, 600);

    socket_=new QTcpSocket(this);

    connect(socket_, &QTcpSocket::readyRead, this, &MainWindow::on_ready_read);
    socket_->connectToHost("127.0.0.1", 12345);
}

void MainWindow::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen borderPen;
    borderPen.setColor(Qt::blue);
    borderPen.setWidth(3);
    borderPen.setStyle(Qt::DashLine);

    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);

    painter.drawRect(0, 0, 800, 600);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::red);
    for (const auto& ball : balls_) {
        painter.drawEllipse(QPointF(ball.pos.x, ball.pos.y), ball.radius, ball.radius);
    }
}

void MainWindow::on_ready_read() {
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
        balls_=snap.balls;

        update();
    }
}