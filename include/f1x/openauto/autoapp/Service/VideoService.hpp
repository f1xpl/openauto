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

#include <memory>
#include <f1x/aasdk/Channel/AV/VideoServiceChannel.hpp>
#include <f1x/aasdk/Channel/AV/IVideoServiceChannelEventHandler.hpp>
#include <f1x/openauto/autoapp/Projection/IVideoOutput.hpp>
#include <f1x/openauto/autoapp/Service/IService.hpp>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace service
{

class VideoService: public aasdk::channel::av::IVideoServiceChannelEventHandler, public IService, public std::enable_shared_from_this<VideoService>
{
public:
    typedef std::shared_ptr<VideoService> Pointer;

    VideoService(boost::asio::io_service& ioService, aasdk::messenger::IMessenger::Pointer messenger, projection::IVideoOutput::Pointer videoOutput);

    void start() override;
    void stop() override;
    void fillFeatures(aasdk::proto::messages::ServiceDiscoveryResponse& response) override;
    void onChannelOpenRequest(const aasdk::proto::messages::ChannelOpenRequest& request) override;
    void onAVChannelSetupRequest(const aasdk::proto::messages::AVChannelSetupRequest& request) override;
    void onAVChannelStartIndication(const aasdk::proto::messages::AVChannelStartIndication& indication) override;
    void onAVChannelStopIndication(const aasdk::proto::messages::AVChannelStopIndication& indication) override;
    void onAVMediaWithTimestampIndication(aasdk::messenger::Timestamp::ValueType timestamp, const aasdk::common::DataConstBuffer& buffer) override;
    void onAVMediaIndication(const aasdk::common::DataConstBuffer& buffer) override;
    void onVideoFocusRequest(const aasdk::proto::messages::VideoFocusRequest& request) override;
    void onChannelError(const aasdk::error::Error& e) override;

private:
    using std::enable_shared_from_this<VideoService>::shared_from_this;
    void sendVideoFocusIndication();

    boost::asio::io_service::strand strand_;
    aasdk::channel::av::VideoServiceChannel::Pointer channel_;
    projection::IVideoOutput::Pointer videoOutput_;
    int32_t session_;
};

}
}
}
}
