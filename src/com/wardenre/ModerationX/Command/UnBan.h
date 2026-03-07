#pragma once

#include <string>

namespace com::wardenre::ModerationX::Command {

    class Unban {
    public:
        static void loadCommand();

    private:
        struct UnbanParams {
            std::string player;
        };
    };

} // namespace com::wardenre::ModerationX::Command
