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

#include <QApplication>
#include <f1x/aasdk/USB/USBHub.hpp>
#include <f1x/aasdk/USB/ConnectedAccessoriesEnumerator.hpp>
#include <f1x/openauto/autoapp/Main.hpp>

namespace f1x
{
namespace openauto
{
namespace autoapp
{

Main::Main(aasdk::usb::IUSBWrapper& usbWrapper, boost::asio::io_service& ioService, configuration::IConfiguration::Pointer configuration)
    : usbWrapper_(usbWrapper)
    , ioService_(ioService)
    , queryFactory_(usbWrapper_, ioService_)
    , queryChainFactory_(usbWrapper_, ioService_, queryFactory_)
    , serviceFactory_(ioService_, configuration)
    , androidAutoEntityFactory_(usbWrapper_, ioService_, configuration, serviceFactory_)
{
    auto usbHub(std::make_shared<aasdk::usb::USBHub>(usbWrapper_, ioService_, queryChainFactory_));
    auto ConnectedAccessoriesEnumerator(std::make_shared<aasdk::usb::ConnectedAccessoriesEnumerator>(usbWrapper_, ioService_, queryChainFactory_));

    app_ = std::make_shared<autoapp::App>(ioService_, androidAutoEntityFactory_,
                                          std::move(usbHub), std::move(ConnectedAccessoriesEnumerator));
}

void Main::start()
{
    app_->start();
}

void Main::stop()
{
    app_->stop();
}

}
}
}
