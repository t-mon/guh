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

#include "guhtouch.h"

GuhTouch::GuhTouch(QObject *parent, int gpio) :
    QObject(parent), m_gpioPin(gpio)
{
    m_touchState = false;
}

bool GuhTouch::enable()
{
    qDebug() << "setup touch sensor on GPIO " << m_gpioPin;
    m_monitor = new GpioMonitor(this);

    m_gpio = new Gpio(this, m_gpioPin);

    if(!m_monitor->addGpio(m_gpio, true)){
        return false;
    }
    connect(m_monitor, &GpioMonitor::changed, this, &GuhTouch::gpioChanged);

    m_monitor->start();
    return true;
}

void GuhTouch::disable()
{
    m_monitor->stop();
}

void GuhTouch::gpioChanged(const int &gpioPin, const int &value)
{
    if (gpioPin == m_gpioPin){
        // check touch state
        bool touchState = QVariant(value).toBool();
        if (m_touchState != touchState) {
            if (touchState) {
                emit handDetected();
            } else {
                emit handDisappeared();
            }
            m_touchState = touchState;
        }
    }
}
