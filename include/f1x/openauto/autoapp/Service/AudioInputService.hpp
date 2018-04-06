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

#include <f1x/aasdk/Channel/AV/AVInputServiceChannel.hpp>
#include <f1x/openauto/autoapp/Service/IService.hpp>
#include <f1x/openauto/autoapp/Projection/IAudioInput.hpp>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace service
{

class AudioInputService: public aasdk::channel::av::IAVInputServiceChannelEventHandler, public IService, public std::enable_shared_from_this<AudioInputService>
{
public:
    typedef std::shared_ptr<AudioInputService> Pointer;

    AudioInputService(boost::asio::io_service& ioService, aasdk::messenger::IMessenger::Pointer messenger, projection::IAudioInput::Pointer audioInput);

    void start() override;
    void stop() override;
    void fillFeatures(aasdk::proto::messages::ServiceDiscoveryResponse& response) override;
    void onChannelOpenRequest(const aasdk::proto::messages::ChannelOpenRequest& request) override;
    void onAVChannelSetupRequest(const aasdk::proto::messages::AVChannelSetupRequest& request) override;
    void onAVInputOpenRequest(const aasdk::proto::messages::AVInputOpenRequest& request) override;
    void onAVMediaAckIndication(const aasdk::proto::messages::AVMediaAckIndication& indication) override;
    void onChannelError(const aasdk::error::Error& e) override;

private:
    using std::enable_shared_from_this<AudioInputService>::shared_from_this;
    void onAudioInputOpenSucceed();
    void onAudioInputDataReady(aasdk::common::Data data);
    void readAudioInput();

    boost::asio::io_service::strand strand_;
    aasdk::channel::av::AVInputServiceChannel::Pointer channel_;
    projection::IAudioInput::Pointer audioInput_;
    int32_t session_;
};

}
}
}
}
