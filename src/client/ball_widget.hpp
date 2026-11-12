#pragma once

#include "types.hpp"

#include <QWidget>

class BallWidget : public QWidget {
    Q_OBJECT
public:
    explicit BallWidget(QWidget *parent = nullptr);

    void update_snapshot(const std::vector<Ball>& balls);

    void clear_field();

public slots:
    void setBallInfoVisible(bool visible);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    std::vector<Ball> balls_;

    bool m_isBallInfoVisible{false};
};