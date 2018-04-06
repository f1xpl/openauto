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

#include <boost/asio.hpp>
#include <f1x/aasdk/Transport/ITransport.hpp>
#include <f1x/openauto/autoapp/Configuration/IConfiguration.hpp>
#include <f1x/openauto/autoapp/Service/IAndroidAutoEntityFactory.hpp>
#include <f1x/openauto/autoapp/Service/IServiceFactory.hpp>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace service
{

class AndroidAutoEntityFactory: public IAndroidAutoEntityFactory
{
public:
    AndroidAutoEntityFactory(boost::asio::io_service& ioService,
                             configuration::IConfiguration::Pointer configuration,
                             IServiceFactory& serviceFactory);

    IAndroidAutoEntity::Pointer create(aasdk::usb::IAOAPDevice::Pointer aoapDevice) override;
    IAndroidAutoEntity::Pointer create(aasdk::tcp::ITCPEndpoint::Pointer tcpEndpoint) override;

private:
    IAndroidAutoEntity::Pointer create(aasdk::transport::ITransport::Pointer transport);

    boost::asio::io_service& ioService_;
    configuration::IConfiguration::Pointer configuration_;
    IServiceFactory& serviceFactory_;
};

}
}
}
}
