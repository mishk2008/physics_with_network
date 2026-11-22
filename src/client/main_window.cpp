#include <regex>

#include "main_window.hpp"
#include "ball_widget.hpp"

MainWindow::MainWindow(QWidget *parent)
: QWidget(parent) {
    setWindowTitle("Moving balls");

    ball_widget_= new BallWidget(this);
    ball_parameters_= new QCheckBox(this);
    ball_parameters_->setChecked(false);

    host_input_= new QLineEdit("127.0.0.1", this);
    port_input_= new QLineEdit("12345", this);
    port_input_->setFixedWidth(60);

    connect_button_= new QPushButton("Connect", this);
    disconnect_button_= new QPushButton("Disconnect", this);
    disconnect_button_->setEnabled(false);

    status_label_= new QLabel("Status : Disconnected", this);

    auto* top_layout{new QHBoxLayout()};
    top_layout->addWidget(new QLabel("Host:", this));
    top_layout->addWidget(host_input_);
    top_layout->addWidget(new QLabel("Port:", this));
    top_layout->addWidget(port_input_);
    top_layout->addWidget(connect_button_);
    top_layout->addWidget(disconnect_button_);
    top_layout->addWidget(new QLabel("Show balls parameters:", this));
    top_layout->addWidget(ball_parameters_);
    top_layout->addWidget(status_label_);
    top_layout->addStretch();

    auto* main_layout{new QVBoxLayout()};
    main_layout->addLayout(top_layout);
    main_layout->addWidget(ball_widget_);
    setLayout(main_layout);

    network_client_ = new TcpNetworkClient(this);

    connect(network_client_, &IClientTransport::snapshot_received, this, [this](const Snapshot& snap) {
        ball_widget_->update_snapshot(snap.balls);
    });

    connect(connect_button_, &QPushButton::clicked, this, [this]() {
        QString host{host_input_->text()};
        std::regex host_pattern(
            "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"
        );
        bool ok{0};
        quint16 port{static_cast<quint16>(port_input_->text().toUShort(&ok))};
        if (!ok) { status_label_->setText("Status : Invalid port");
        } else if (!(std::regex_match(host.toStdString(), host_pattern)) && host.toStdString() != "localhost") { status_label_->setText("Status : Invalid host");
        } else {
            status_label_->setText(QString("Status : Connecting to %1:%2...").arg(host).arg(port));
            network_client_->connect_to_server(host, port);
        }
    });

    connect(disconnect_button_, &QPushButton::clicked, this, [this]() {
        network_client_->disconnect_from_server();

        status_label_->setText("Status : Disconnected");
        connect_button_->setEnabled(true);
        disconnect_button_->setEnabled(false);

        ball_widget_->clear_field();
    });

    connect(ball_parameters_, &QCheckBox::toggled, ball_widget_, &BallWidget::setBallInfoVisible);

    connect(network_client_, &IClientTransport::connected, this, [this]() {
        status_label_->setText("Status : Connected");
        connect_button_->setEnabled(false);
        disconnect_button_->setEnabled(true);
    });

    connect(network_client_, &IClientTransport::disconnected, this, [this]() {
        if (!status_label_->text().startsWith("Status : Error")) {
            status_label_->setText("Status : Disconnected");            
        }
        connect_button_->setEnabled(true);
        disconnect_button_->setEnabled(false);
        ball_widget_->clear_field();
    });

    connect(network_client_, &IClientTransport::error_occured, this, [this](const QString& error_msg) {
        status_label_->setText("Status : Error (" + error_msg + ')');
    });
}
