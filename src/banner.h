#pragma once

#include <QLabel>
#include <QObject>
#include <QMouseEvent>

class Banner : public QLabel
{
    Q_OBJECT
public:
    explicit Banner(QWidget *parent = 0);
    void mousePressEvent( QMouseEvent * e);
signals:
    void onClick();
public slots:
    
};
