#include "ball_widget.hpp"

#include <QPainter>

BallWidget::BallWidget(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedSize(800, 600);
}

void BallWidget::update_snapshot(const std::vector<Ball>& balls) {
    balls_ = balls;
    update();
}

void BallWidget::clear_field() {
    balls_.clear();
    update();
}

void BallWidget::setBallInfoVisible(bool visible) {
    m_isBallInfoVisible = visible;
    update();
}

void BallWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
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

        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::red);
        painter.drawEllipse(QPointF(ball.pos.x, ball.pos.y), ball.radius, ball.radius);

        if (m_isBallInfoVisible) {
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
        }
    }
}
