#pragma once

#include "types.hpp"
#include "network_client.hpp"
#include "ball_widget.hpp"

#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QVBoxLayout>

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent=nullptr);
    ~MainWindow() override=default;

private:
    IClientTransport* network_client_;
    BallWidget* ball_widget_;
    QCheckBox* ball_parameters_;

    QLineEdit* port_input_;
    QLineEdit* host_input_;
    QPushButton* connect_button_;
    QPushButton* disconnect_button_;
    QLabel* status_label_;
};