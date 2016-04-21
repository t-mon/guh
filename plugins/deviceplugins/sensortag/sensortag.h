#ifndef SENSORTAG_H
#define SENSORTAG_H

#include <QObject>
#include <QLowEnergyController>
#include <QBluetoothUuid>

#include "bluetooth/bluetoothlowenergydevice.h"
#include "extern-plugininfo.h"

class SensorTag : public BluetoothLowEnergyDevice
{
    Q_OBJECT
public:
    explicit SensorTag(const QBluetoothDeviceInfo &deviceInfo, const QLowEnergyController::RemoteAddressType &addressType, QObject *parent);

private:
    QLowEnergyService *m_temperatureService;

signals:

private slots:
    void onServiceDiscoveryFinished();

    // Service slots
    void serviceStateChanged(const QLowEnergyService::ServiceState &state);
//    void serviceCharacteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
//    void confirmedCharacteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
//    void confirmedDescriptorWritten(const QLowEnergyDescriptor &descriptor, const QByteArray &value);
//    void serviceError(const QLowEnergyService::ServiceError &error);


};

#endif // SENSORTAG_H
