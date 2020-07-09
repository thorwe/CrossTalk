#include "banner.h"

#include <QtGui/QMouseEvent>
#include <QtGui/QPixmap>

//! Constructor
/*!
 * \brief Banner::Banner Creates a clickable QLabel
 * \param parent
 */
Banner::Banner(QWidget *parent)
{
    // MouseTracking must be set to true to make Image clickable
    this->setMouseTracking(true);
    this->setParent(parent);
    this->setVisible(true);
}

//! Catches the event, emits onClick
/*!
 * \brief Banner::mousePressEvent called by the Qt Event System
 */
void Banner::mousePressEvent(QMouseEvent *)
{
    emit onClick();
}
