#pragma once

#include <QtWidgetS/QLabel>
#include <QtCore/QObject>
#include <QtGui/QMouseEvent>

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
