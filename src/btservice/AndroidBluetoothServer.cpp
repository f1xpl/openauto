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

#include <f1x/openauto/Common/Log.hpp>
#include <f1x/openauto/btservice/AndroidBluetoothServer.hpp>

namespace f1x
{
namespace openauto
{
namespace btservice
{

AndroidBluetoothServer::AndroidBluetoothServer()
    : rfcommServer_(std::make_unique<QBluetoothServer>(QBluetoothServiceInfo::RfcommProtocol, this))
{
    connect(rfcommServer_.get(), &QBluetoothServer::newConnection, this, &AndroidBluetoothServer::onClientConnected);
}

bool AndroidBluetoothServer::start(const QBluetoothAddress& address, uint16_t portNumber)
{
    return rfcommServer_->listen(address, portNumber);
}

void AndroidBluetoothServer::onClientConnected()
{
    auto socket = rfcommServer_->nextPendingConnection();

    if(socket != nullptr)
    {
        OPENAUTO_LOG(info) << "[AndroidBluetoothServer] rfcomm client connected, peer name: " << socket->peerName().toStdString();
    }
    else
    {
        OPENAUTO_LOG(error) << "[AndroidBluetoothServer] received null socket during client connection.";
    }
}

}
}
}
