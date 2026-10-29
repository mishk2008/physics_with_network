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

    for (const auto& ball : balls_) {
        QString b_pos_x=QString("x: %1").arg(ball.pos.x, 0, 'f', 1);
        QString b_pos_y=QString("y: %1").arg(ball.pos.y, 0, 'f', 1);
        double velocity{sqrt(std::pow(ball.vel.x, 2)+std::pow(ball.vel.y, 2))};
        QString b_vel=QString("vel: %1").arg(velocity, 0, 'f', 1);
        QString b_mass=QString("mass: %1").arg(ball.mass, 0, 'f', 1);
        QString b_id=QString("id: %0").arg(ball.id);

        painter.setPen(Qt::green);

        painter.drawLine(ball.pos.x, ball.pos.y, ball.pos.x+10, ball.pos.y+10);
        painter.drawLine(ball.pos.x+10, ball.pos.y+10, ball.pos.x+60, ball.pos.y+10);

        int textX = ball.pos.x + 35;
        int textY = ball.pos.y + 10;
        
        painter.drawText(textX, textY, b_id);        
        painter.drawText(textX, textY + 12, b_vel);        
        painter.drawText(textX, textY + 24, b_mass); 
        painter.drawText(textX, textY + 36, b_pos_x);
        painter.drawText(textX, textY + 48, b_pos_y);

        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::red);
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