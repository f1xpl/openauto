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
#include <f1x/aasdk/Channel/Control/ControlServiceChannel.hpp>
#include <f1x/aasdk/Channel/Control/IControlServiceChannelEventHandler.hpp>
#include <f1x/aasdk/Channel/AV/VideoServiceChannel.hpp>
#include <f1x/openauto/autoapp/Configuration/IConfiguration.hpp>
#include <f1x/openauto/autoapp/Projection/IAndroidAutoEntity.hpp>
#include <f1x/openauto/autoapp/Projection/IServiceFactory.hpp>
#include <f1x/openauto/autoapp/Projection/IPinger.hpp>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace projection
{

class AndroidAutoEntity: public IAndroidAutoEntity, public aasdk::channel::control::IControlServiceChannelEventHandler, public std::enable_shared_from_this<AndroidAutoEntity>
{
public:
    AndroidAutoEntity(boost::asio::io_service& ioService,
                      aasdk::messenger::ICryptor::Pointer cryptor,
                      aasdk::transport::ITransport::Pointer transport,
                      configuration::IConfiguration::Pointer configuration,
                      IServiceFactory& serviceFactory,
                      IPinger::Pointer pinger);
    ~AndroidAutoEntity() override;

    void start(IAndroidAutoEntityEventHandler& eventHandler) override;
    void stop() override;
    void onVersionResponse(uint16_t majorCode, uint16_t minorCode, aasdk::proto::enums::VersionResponseStatus::Enum status) override;
    void onHandshake(const aasdk::common::DataConstBuffer& payload) override;
    void onServiceDiscoveryRequest(const aasdk::proto::messages::ServiceDiscoveryRequest& request) override;
    void onAudioFocusRequest(const aasdk::proto::messages::AudioFocusRequest& request) override;
    void onShutdownRequest(const aasdk::proto::messages::ShutdownRequest& request) override;
    void onShutdownResponse(const aasdk::proto::messages::ShutdownResponse& response) override;
    void onNavigationFocusRequest(const aasdk::proto::messages::NavigationFocusRequest& request) override;
    void onPingResponse(const aasdk::proto::messages::PingResponse& response) override;
    void onChannelError(const aasdk::error::Error& e) override;

private:
    using std::enable_shared_from_this<AndroidAutoEntity>::shared_from_this;
    void triggerQuit();
    void schedulePing();
    void sendPing();

    boost::asio::io_service::strand strand_;
    aasdk::messenger::ICryptor::Pointer cryptor_;
    aasdk::transport::ITransport::Pointer transport_;
    configuration::IConfiguration::Pointer configuration_;
    IServiceFactory& serviceFactory_;
    IPinger::Pointer pinger_;
    aasdk::messenger::IMessenger::Pointer messenger_;
    aasdk::channel::control::ControlServiceChannel::Pointer controlServiceChannel_;
    ServiceList serviceList_;
    IAndroidAutoEntityEventHandler* eventHandler_;
};

}
}
}
}
