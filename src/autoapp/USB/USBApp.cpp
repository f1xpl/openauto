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

#include <thread>
#include <f1x/openauto/autoapp/USB/USBApp.hpp>
#include <f1x/openauto/Common/Log.hpp>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace usb
{

USBApp::USBApp(boost::asio::io_service& ioService, projection::IAndroidAutoEntityFactory& androidAutoEntityFactory, aasdk::usb::IUSBHub::Pointer usbHub)
    : ioService_(ioService)
    , strand_(ioService_)
    , androidAutoEntityFactory_(androidAutoEntityFactory)
    , usbHub_(std::move(usbHub))
    , isStopped_(false)
{

}

void USBApp::start()
{
    strand_.dispatch([this, self = this->shared_from_this()]() {
        this->waitForDevice();
    });
}

void USBApp::stop()
{
    strand_.dispatch([this, self = this->shared_from_this()]() {
        isStopped_ = true;
        usbHub_->cancel();

        if(androidAutoEntity_ != nullptr)
        {
            androidAutoEntity_->stop();
        }
    });
}

void USBApp::aoapDeviceHandler(aasdk::usb::DeviceHandle deviceHandle)
{
    OPENAUTO_LOG(info) << "[USBApp] Device connected.";

    if(androidAutoEntity_ == nullptr)
    {
        try
        {
            androidAutoEntity_ = androidAutoEntityFactory_.create(std::move(deviceHandle));
            androidAutoEntity_->start(*this);
        }
        catch(const aasdk::error::Error& error)
        {
            OPENAUTO_LOG(error) << "[USBApp] AndroidAutoEntity create error: " << error.what();

            androidAutoEntity_.reset();
            this->waitForDevice();
        }
    }
    else
    {
        OPENAUTO_LOG(warning) << "[USBApp] android auto entity is still running.";
    }
}

void USBApp::waitForDevice()
{
    OPENAUTO_LOG(info) << "[USBApp] Waiting for device...";

    auto promise = aasdk::usb::IUSBHub::Promise::defer(strand_);
    promise->then(std::bind(&USBApp::aoapDeviceHandler, this->shared_from_this(), std::placeholders::_1),
                  std::bind(&USBApp::onUSBHubError, this->shared_from_this(), std::placeholders::_1));
    usbHub_->start(std::move(promise));
}

void USBApp::onAndroidAutoQuit()
{
    strand_.dispatch([this, self = this->shared_from_this()]() {
        OPENAUTO_LOG(info) << "[USBApp] quit.";

        androidAutoEntity_->stop();
        androidAutoEntity_.reset();

        if(!isStopped_)
        {
            this->waitForDevice();
        }
    });
}

void USBApp::onUSBHubError(const aasdk::error::Error& error)
{
    OPENAUTO_LOG(error) << "[USBApp] usb hub error: " << error.what();

    if(error.getCode() == aasdk::error::ErrorCode::OPERATION_ABORTED ||
       error.getCode() == aasdk::error::ErrorCode::OPERATION_IN_PROGRESS)
    {
        this->waitForDevice();
    }
}

}
}
}
}
