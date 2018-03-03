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

#ifdef USE_OMX

extern "C"
{
#include <bcm_host.h>
}

#include <f1x/aasdk/Common/Data.hpp>
#include <f1x/openauto/autoapp/Projection/OMXVideoOutput.hpp>
#include <f1x/openauto/Common/Log.hpp>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace projection
{

namespace VideoComponent
{
    static constexpr uint32_t DECODER = 0;
    static constexpr uint32_t RENDERER = 1;
    static constexpr uint32_t CLOCK = 2;
    static constexpr uint32_t SCHEDULER = 3;
}

OMXVideoOutput::OMXVideoOutput(configuration::IConfiguration::Pointer configuration)
    : VideoOutput(std::move(configuration))
    , isActive_(false)
    , portSettingsChanged_(false)
    , client_(nullptr)
{
    memset(components_, 0, sizeof(components_));
    memset(tunnels_, 0, sizeof(tunnels_));
}

bool OMXVideoOutput::open()
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    OPENAUTO_LOG(info) << "[OMXVideoOutput] open.";

    bcm_host_init();
    if(OMX_Init() != OMX_ErrorNone)
    {
        OPENAUTO_LOG(error) << "[OMXVideoOutput] omx init failed.";
        return false;
    }

    client_ = ilclient_init();
    if(client_ == nullptr)
    {
        OPENAUTO_LOG(error) << "[OMXVideoOutput] ilclient init failed.";
        return false;
    }

    if(!this->createComponents())
    {
        return false;
    }

    if(!this->setupTunnels())
    {
        OPENAUTO_LOG(error) << "[OMXVideoOutput] setup tunnels failed.";
        return false;
    }

    ilclient_change_component_state(components_[VideoComponent::CLOCK], OMX_StateExecuting);
    ilclient_change_component_state(components_[VideoComponent::DECODER], OMX_StateIdle);

    if(!this->enablePortBuffers())
    {
        OPENAUTO_LOG(error) << "[OMXVideoOutput] enable port buffers failed.";
        return false;
    }

    isActive_ = true;
    return true;
}

bool OMXVideoOutput::init()
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    OPENAUTO_LOG(info) << "[OMXVideoOutput] init, state: " << isActive_;
    ilclient_change_component_state(components_[VideoComponent::DECODER], OMX_StateExecuting);
    
    return this->setupDisplayRegion();
}

bool OMXVideoOutput::setupDisplayRegion()
{
    OMX_CONFIG_DISPLAYREGIONTYPE displayRegion;
    displayRegion.nSize = sizeof(OMX_CONFIG_DISPLAYREGIONTYPE);
    displayRegion.nVersion.nVersion = OMX_VERSION;
    displayRegion.nPortIndex = 90;
    displayRegion.layer = static_cast<OMX_S32>(configuration_->getOMXLayerIndex());
    displayRegion.fullscreen = OMX_TRUE;
    displayRegion.noaspect = OMX_TRUE;
    displayRegion.set = static_cast<OMX_DISPLAYSETTYPE >(OMX_DISPLAY_SET_FULLSCREEN | OMX_DISPLAY_SET_NOASPECT | OMX_DISPLAY_SET_LAYER);    

    return OMX_SetConfig(ilclient_get_handle(components_[VideoComponent::RENDERER]), OMX_IndexConfigDisplayRegion, &displayRegion) == OMX_ErrorNone;
}

void OMXVideoOutput::write(uint64_t timestamp, const aasdk::common::DataConstBuffer& buffer)
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    size_t writeSize = 0;

    while(isActive_ && writeSize < buffer.size)
    {
        OMX_BUFFERHEADERTYPE* buf = ilclient_get_input_buffer(components_[VideoComponent::DECODER], 130, 1);

        if(buf == nullptr)
        {
            break;
        }
        else
        {
            aasdk::common::DataConstBuffer currentBuffer(buffer.cdata, buffer.size, writeSize);
            buf->nFilledLen = std::min<size_t>(buf->nAllocLen, currentBuffer.size);
            memcpy(buf->pBuffer, &currentBuffer.cdata[0], buf->nFilledLen);
            buf->nTimeStamp = omx_ticks_from_s64(timestamp / 1000000);
            buf->nOffset = 0;           
            
            writeSize += buf->nFilledLen;
            
            if(timestamp == 0)
            {
              buf->nFlags = OMX_BUFFERFLAG_STARTTIME;
            }

            if(!portSettingsChanged_ && ilclient_remove_event(components_[VideoComponent::DECODER], OMX_EventPortSettingsChanged, 131, 0, 0, 1) == 0)
            {
                portSettingsChanged_ = true;

                if(ilclient_setup_tunnel(&tunnels_[0], 0, 0) != 0)
                {
                    break;
                }

                ilclient_change_component_state(components_[VideoComponent::SCHEDULER], OMX_StateExecuting);
                if(ilclient_setup_tunnel(&tunnels_[1], 0, 1000) != 0)
                {
                    break;
                }

                ilclient_change_component_state(components_[VideoComponent::RENDERER], OMX_StateExecuting);
            }

            if(OMX_EmptyThisBuffer(ILC_GET_HANDLE(components_[VideoComponent::DECODER]), buf) != OMX_ErrorNone)
            {
                break;
            }
        }
    }
}

void OMXVideoOutput::stop()
{
    OPENAUTO_LOG(info) << "[OMXVideoOutput] stop.";

    std::lock_guard<decltype(mutex_)> lock(mutex_);

    if(isActive_)
    {
        isActive_ = false;

        ilclient_disable_tunnel(&tunnels_[0]);
        ilclient_disable_tunnel(&tunnels_[1]);
        ilclient_disable_tunnel(&tunnels_[2]);
        ilclient_disable_port_buffers(components_[VideoComponent::DECODER], 130, NULL, NULL, NULL);
        ilclient_teardown_tunnels(tunnels_);

        ilclient_state_transition(components_, OMX_StateIdle);
        ilclient_state_transition(components_, OMX_StateLoaded);

        ilclient_cleanup_components(components_);
        OMX_Deinit();
        ilclient_destroy(client_);
    }
}

bool OMXVideoOutput::createComponents()
{
    if(ilclient_create_component(client_, &components_[VideoComponent::DECODER], "video_decode", static_cast<ILCLIENT_CREATE_FLAGS_T>(ILCLIENT_DISABLE_ALL_PORTS | ILCLIENT_ENABLE_INPUT_BUFFERS)) != 0)
    {
        OPENAUTO_LOG(error) << "[OMXVideoOutput] video decode component creation failed.";
        return false;
    }

    if(ilclient_create_component(client_, &components_[VideoComponent::RENDERER], "video_render", ILCLIENT_DISABLE_ALL_PORTS) != 0)
    {
        OPENAUTO_LOG(error) << "[OMXVideoOutput] video renderer component creation failed.";
        return false;
    }

    if(ilclient_create_component(client_, &components_[VideoComponent::CLOCK], "clock", ILCLIENT_DISABLE_ALL_PORTS) != 0)
    {
        OPENAUTO_LOG(error) << "[OMXVideoOutput] clock component creation failed.";
        return false;
    }

    if(!this->initClock())
    {
        OPENAUTO_LOG(error) << "[OMXVideoOutput] clock init failed.";
        return false;
    }

    if(ilclient_create_component(client_, &components_[VideoComponent::SCHEDULER], "video_scheduler", ILCLIENT_DISABLE_ALL_PORTS) != 0)
    {
        OPENAUTO_LOG(error) << "[OMXVideoOutput] video scheduler component creation failed.";
        return false;
    }

    return true;
}

bool OMXVideoOutput::initClock()
{
    OMX_TIME_CONFIG_CLOCKSTATETYPE cstate;
    memset(&cstate, 0, sizeof(cstate));
    cstate.nSize = sizeof(cstate);
    cstate.nVersion.nVersion = OMX_VERSION;
    cstate.eState = OMX_TIME_ClockStateWaitingForStartTime;
    cstate.nWaitMask = 1;

    return OMX_SetParameter(ILC_GET_HANDLE(components_[VideoComponent::CLOCK]), OMX_IndexConfigTimeClockState, &cstate) == OMX_ErrorNone;
}

bool OMXVideoOutput::setupTunnels()
{
    set_tunnel(&tunnels_[0], components_[VideoComponent::DECODER], 131, components_[VideoComponent::SCHEDULER], 10);
    set_tunnel(&tunnels_[1], components_[VideoComponent::SCHEDULER], 11, components_[VideoComponent::RENDERER], 90);
    set_tunnel(&tunnels_[2], components_[VideoComponent::CLOCK], 80, components_[VideoComponent::SCHEDULER], 12);

    return ilclient_setup_tunnel(&tunnels_[2], 0, 0) == 0;
}

bool OMXVideoOutput::enablePortBuffers()
{
    OMX_VIDEO_PARAM_PORTFORMATTYPE format;
    memset(&format, 0, sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE));
    format.nSize = sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE);
    format.nVersion.nVersion = OMX_VERSION;
    format.nPortIndex = 130;
    format.eCompressionFormat = OMX_VIDEO_CodingAVC;

    return OMX_SetParameter(ILC_GET_HANDLE(components_[VideoComponent::DECODER]), OMX_IndexParamVideoPortFormat, &format) == OMX_ErrorNone &&
           ilclient_enable_port_buffers(components_[VideoComponent::DECODER], 130, NULL, NULL, NULL) == 0;
}

}
}
}
}

#endif
