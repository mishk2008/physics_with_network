#pragma once

#include "types.hpp"

#include <QWidget>
#include <QTcpSocket>

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent=nullptr);
    ~MainWindow() override=default;

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void on_ready_read();

private:
    QTcpSocket* socket_;
    QByteArray network_buffer_;
    std::vector<Ball> balls_;
};