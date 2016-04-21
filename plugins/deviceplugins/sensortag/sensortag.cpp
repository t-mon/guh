#include "sensortag.h"


SensorTag::SensorTag(const QBluetoothDeviceInfo &deviceInfo, const QLowEnergyController::RemoteAddressType &addressType, QObject *parent) :
    BluetoothLowEnergyDevice(deviceInfo, addressType, parent)
{
    connect(this, SIGNAL(servicesDiscoveryFinished()), this, SLOT(onServiceDiscoveryFinished()));
}

void SensorTag::onServiceDiscoveryFinished()
{
    qCDebug(dcSensorTag()) << "Service discovery finished";

    QBluetoothUuid temperatureServiceUuid = QBluetoothUuid(QUuid("f000aa00-0451-4000-b000-000000000000"));


    foreach (const QBluetoothUuid &serviceUuid, controller()->services())
        qCDebug(dcSensorTag()) << serviceUuid.toString();

    if (controller()->services().contains(temperatureServiceUuid)) {
        qCDebug(dcSensorTag()) << "Register temperature service" << temperatureServiceUuid.toString();

        m_temperatureService = controller()->createServiceObject(temperatureServiceUuid, this);

        connect(m_temperatureService, SIGNAL(stateChanged(QLowEnergyService::ServiceState)), this, SLOT(serviceStateChanged(QLowEnergyService::ServiceState)));
//        connect(m_temperatureService, SIGNAL(characteristicChanged(QLowEnergyCharacteristic, QByteArray)), this, SLOT(serviceCharacteristicChanged(QLowEnergyCharacteristic, QByteArray)));
//        connect(m_temperatureService, SIGNAL(characteristicWritten(QLowEnergyCharacteristic, QByteArray)), this, SLOT(confirmedCharacteristicWritten(QLowEnergyCharacteristic, QByteArray)));
//        connect(m_temperatureService, SIGNAL(descriptorWritten(QLowEnergyDescriptor, QByteArray)), this, SLOT(confirmedDescriptorWritten(QLowEnergyDescriptor, QByteArray)));
//        connect(m_temperatureService, SIGNAL(error(QLowEnergyService::ServiceError)), this, SLOT(serviceError(QLowEnergyService::ServiceError)));

        m_temperatureService->discoverDetails();


    }

}

void SensorTag::serviceStateChanged(const QLowEnergyService::ServiceState &state)
{
    QLowEnergyService *service =static_cast<QLowEnergyService *>(sender());

    switch (state) {
    case QLowEnergyService::DiscoveringServices:
        if (service->serviceUuid() == m_temperatureService->serviceUuid()) {
            qCDebug(dcSensorTag()) << "Start discovering temperature service";
        }
        break;
    case QLowEnergyService::ServiceDiscovered:
        // check which service is discovered
        if (service->serviceUuid() == m_temperatureService->serviceUuid()) {
            qCDebug(dcSensorTag()) << "Temperature service discovered.";

            foreach (const QLowEnergyCharacteristic &characterisic, m_temperatureService->characteristics()) {
                qCDebug(dcSensorTag()) << " -> Characteristic" << characterisic.name() << characterisic.handle();
                qCDebug(dcSensorTag()) << "     " << service->readCharacteristic(characterisic);
            }

        }

        break;
    default:
        break;
    }


}

