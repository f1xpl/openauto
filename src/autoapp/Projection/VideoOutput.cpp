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

#include <f1x/openauto/autoapp/Projection/VideoOutput.hpp>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace projection
{

VideoOutput::VideoOutput(configuration::IConfiguration::Pointer configuration)
    : configuration_(std::move(configuration))
{

}

aasdk::proto::enums::VideoFPS::Enum VideoOutput::getVideoFPS() const
{
    return configuration_->getVideoFPS();
}

aasdk::proto::enums::VideoResolution::Enum VideoOutput::getVideoResolution() const
{
    return configuration_->getVideoResolution();
}

size_t VideoOutput::getScreenDPI() const
{
    return configuration_->getScreenDPI();
}

QRect VideoOutput::getVideoMargins() const
{
    return configuration_->getVideoMargins();
}

}
}
}
}
