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

#include <f1x/openauto/autoapp/Projection/RemoteBluetoothDevice.hpp>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace projection
{

RemoteBluetoothDevice::RemoteBluetoothDevice(const std::string& address)
    : address_(address)
{

}

void RemoteBluetoothDevice::stop()
{

}

bool RemoteBluetoothDevice::isPaired(const std::string&) const
{
    return true;
}

void RemoteBluetoothDevice::pair(const std::string&, PairingPromise::Pointer promise)
{
    promise->resolve();
}

std::string RemoteBluetoothDevice::getLocalAddress() const
{
    return address_;
}

bool RemoteBluetoothDevice::isAvailable() const
{
    return true;
}

}
}
}
}
