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

#include <vector>
#include <memory>
#include <aasdk_proto/ServiceDiscoveryResponseMessage.pb.h>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace service
{

class IService
{
public:
    typedef std::shared_ptr<IService> Pointer;

    virtual ~IService() = default;

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void fillFeatures(aasdk::proto::messages::ServiceDiscoveryResponse& response) = 0;
};

typedef std::vector<IService::Pointer> ServiceList;

}
}
}
}
