/*
*  This file is part of openauto project.
*  Copyright (C) 2018 f1x.studio (Michal Szwaj)
*
*  openauto is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 3 of the License, or
*  (at your option) any later version.

*  openauto is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with openauto. If not, see <http://www.gnu.org/licenses/>.
*/

#include <QBluetoothLocalDevice>
#include <f1x/openauto/autoapp/Projection/IBluetoothDevice.hpp>

#pragma once

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace projection
{

class LocalBluetoothDevice: public QObject, public IBluetoothDevice
{
    Q_OBJECT

public:
    LocalBluetoothDevice();

    void stop() override;
    bool isPaired(const std::string& address) const override;
    void pair(const std::string& address, PairingPromise::Pointer promise) override;
    std::string getLocalAddress() const override;
    bool isAvailable() const override;

signals:
    void startPairing(const QString& address, PairingPromise::Pointer promise);

private slots:
    void createBluetoothLocalDevice();
    void onStartPairing(const QString& address, PairingPromise::Pointer promise);
    void onPairingDisplayConfirmation(const QBluetoothAddress &address, QString pin);
    void onPairingDisplayPinCode(const QBluetoothAddress &address, QString pin);
    void onPairingFinished(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing);
    void onError(QBluetoothLocalDevice::Error error);
    void onHostModeStateChanged(QBluetoothLocalDevice::HostMode state);

private:
    mutable std::mutex mutex_;
    std::unique_ptr<QBluetoothLocalDevice> localDevice_;
    PairingPromise::Pointer pairingPromise_;
    QBluetoothAddress pairingAddress_;
};

}
}
}
}
