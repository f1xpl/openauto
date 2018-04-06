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

#pragma once

#include <f1x/aasdk/USB/IUSBHub.hpp>
#include <f1x/aasdk/USB/IConnectedAccessoriesEnumerator.hpp>
#include <f1x/aasdk/USB/USBWrapper.hpp>
#include <f1x/aasdk/TCP/ITCPWrapper.hpp>
#include <f1x/aasdk/TCP/ITCPEndpoint.hpp>
#include <f1x/openauto/autoapp/Service/IAndroidAutoEntityEventHandler.hpp>
#include <f1x/openauto/autoapp/Service/IAndroidAutoEntityFactory.hpp>

namespace f1x
{
namespace openauto
{
namespace autoapp
{

class App: public service::IAndroidAutoEntityEventHandler, public std::enable_shared_from_this<App>
{
public:
    typedef std::shared_ptr<App> Pointer;

    App(boost::asio::io_service& ioService, aasdk::usb::USBWrapper& usbWrapper, aasdk::tcp::ITCPWrapper& tcpWrapper, service::IAndroidAutoEntityFactory& androidAutoEntityFactory,
        aasdk::usb::IUSBHub::Pointer usbHub, aasdk::usb::IConnectedAccessoriesEnumerator::Pointer connectedAccessoriesEnumerator);

    void waitForUSBDevice();
    void start(aasdk::tcp::ITCPEndpoint::SocketPointer socket);
    void stop();
    void onAndroidAutoQuit() override;

private:
    using std::enable_shared_from_this<App>::shared_from_this;
    void enumerateDevices();
    void waitForDevice();
    void aoapDeviceHandler(aasdk::usb::DeviceHandle deviceHandle);
    void onUSBHubError(const aasdk::error::Error& error);

    boost::asio::io_service& ioService_;
    aasdk::usb::USBWrapper& usbWrapper_;
    aasdk::tcp::ITCPWrapper& tcpWrapper_;
    boost::asio::io_service::strand strand_;
    service::IAndroidAutoEntityFactory& androidAutoEntityFactory_;
    aasdk::usb::IUSBHub::Pointer usbHub_;
    aasdk::usb::IConnectedAccessoriesEnumerator::Pointer connectedAccessoriesEnumerator_;
    service::IAndroidAutoEntity::Pointer androidAutoEntity_;
    bool isStopped_;
};

}
}
}
