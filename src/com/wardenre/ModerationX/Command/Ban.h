#include "mc/world/actor/player/Player.h"
#include <ll/api/command/CommandRegistrar.h>
#include <mc/server/commands/CommandSelector.h>
#include <string>

namespace com::wardenre::ModerationX::Command {
    using namespace ll::command;
    class Ban {
        public:
        static void loadCommand(CommandRegistrar& registrar);
        private:
        enum BanOperation: int {
            ip,
            soft,
            perm,
            norm
        };
        struct BanParams {
            CommandSelector<Player> player;
            BanOperation operation;
            int time;
            std::string reason;
        };
    };
}