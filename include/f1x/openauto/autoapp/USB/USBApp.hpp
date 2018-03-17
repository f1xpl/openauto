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
#include <f1x/openauto/autoapp/Projection/IAndroidAutoEntityEventHandler.hpp>
#include <f1x/openauto/autoapp/Projection/IAndroidAutoEntityFactory.hpp>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace usb
{

class USBApp: public projection::IAndroidAutoEntityEventHandler, public std::enable_shared_from_this<USBApp>
{
public:
    typedef std::shared_ptr<USBApp> Pointer;

    USBApp(boost::asio::io_service& ioService, projection::IAndroidAutoEntityFactory& androidAutoEntityFactory,
           aasdk::usb::IUSBHub::Pointer usbHub, aasdk::usb::IConnectedAccessoriesEnumerator::Pointer connectedAccessoriesEnumerator);

    void start();
    void stop();
    void onAndroidAutoQuit() override;

private:
    using std::enable_shared_from_this<USBApp>::shared_from_this;

    void enumerateDevices();
    void waitForDevice();
    void aoapDeviceHandler(aasdk::usb::DeviceHandle deviceHandle);
    void onUSBHubError(const aasdk::error::Error& error);

    boost::asio::io_service& ioService_;
    boost::asio::io_service::strand strand_;
    projection::IAndroidAutoEntityFactory& androidAutoEntityFactory_;
    aasdk::usb::IUSBHub::Pointer usbHub_;
    aasdk::usb::IConnectedAccessoriesEnumerator::Pointer connectedAccessoriesEnumerator_;
    projection::IAndroidAutoEntity::Pointer androidAutoEntity_;
    bool isStopped_;
};

}
}
}
}
