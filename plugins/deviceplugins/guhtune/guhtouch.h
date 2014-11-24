/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  This file is part of guh.                                              *
 *                                                                         *
 *  Guh is free software: you can redistribute it and/or modify            *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, version 2 of the License.                *
 *                                                                         *
 *  Guh is distributed in the hope that it will be useful,                 *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with guh. If not, see <http://www.gnu.org/licenses/>.            *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GUHTOUCH_H
#define GUHTOUCH_H

#include <QObject>
#include <QDebug>

#include "hardware/gpiomonitor.h"

class GuhTouch : public QObject
{
    Q_OBJECT
public:
    explicit GuhTouch(QObject *parent = 0, int gpio = 17);
    bool enable();
    void disable();

private:
    GpioMonitor *m_monitor;
    int m_gpioPin;
    Gpio *m_gpio;

    bool m_touchState;

signals:
    void handDetected();
    void handDisappeared();

private slots:
    void gpioChanged(const int &gpioPin, const int &value);

};

#endif // GUHTOUCH_H
