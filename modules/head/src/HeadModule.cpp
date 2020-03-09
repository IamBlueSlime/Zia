/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** HTTPModHeadModuleule.cpp
*/

#include <sstream>
#include <string>
#include "zia/common/Network.hpp"
#include "openZia/Pipeline.hpp"
#include "zia/common/BufferUtils.hpp"
#include "HeadModule.hpp"

namespace Zia::Head {

    HeadModule::HeadModule() : _logger("HeadModule") {}

    void HeadModule::onRegisterCallbacks(oZ::Pipeline &pipeline) {
        pipeline.registerCallback(
                oZ::State::Completed, oZ::Priority::Independent, this, &HeadModule::onPipelineCompleted
        );
    }

    bool HeadModule::onPipelineCompleted(oZ::Context &context) {
        if (context.getRequest().getMethod() == oZ::HTTP::Method::Head) {
            this->_logger.debug("Request method is HEAD, stripping body.");
            context.getResponse().getBody().clear();
        }

        return true;
    }

}

OPEN_ZIA_MAKE_ENTRY_POINT(Zia::Head::HeadModule)
