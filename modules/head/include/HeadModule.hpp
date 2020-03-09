/*
** EPITECH PROJECT, 2020
** CPP_zia_2019
** File description:
** head HeadModule.hpp
*/

#pragma once

#include <regex>
#include "openZia/IModule.hpp"
#include "openZia/HTTP.hpp"
#include "zia/common/Logger.hpp"

namespace Zia::Head {

    class HeadModule: public oZ::IModule {
    public:
        HeadModule();

        void onRegisterCallbacks(oZ::Pipeline &pipeline) override;
        const char *getName() const override { return "Head"; }

    protected:
    private:
        Logger _logger;

        bool onPipelineCompleted(oZ::Context &context);
    };
}