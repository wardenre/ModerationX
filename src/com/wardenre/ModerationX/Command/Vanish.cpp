#include "com/wardenre/ModerationX/Command/Vanish.h"
#include "com/wardenre/ModerationX/Functions/VanishManager.h"

#include "ll/api/command/CommandHandle.h"
#include "ll/api/command/CommandRegistrar.h"

#include "mc/world/actor/player/Player.h"
#include "mc/server/commands/CommandOrigin.h"
#include "mc/server/commands/CommandOutput.h"

namespace com::wardenre::ModerationX::Command {
    using namespace ll::command;

    void Vanish::loadCommand() {
        auto& vanish = CommandRegistrar::getInstance(false).getOrCreateCommand("vanish", "Toggle vanish mode", CommandPermissionLevel::GameDirectors);
        vanish.alias("v");

        vanish.overload()
            .execute([](CommandOrigin const& origin, CommandOutput& output) {
                auto* entity = origin.getEntity();
                if (!entity || !entity->isPlayer()) {
                    output.error("This command can only be used by a player.");
                    return;
                }

                auto* player = static_cast<Player*>(entity);
                auto& vm = Functions::VanishManager::getInstance();

                if (vm.isVanished(*player)) {
                    vm.unvanish(*player);
                    output.success("§aVanish §cOFF§a — you are now visible.");
                } else {
                    vm.vanish(*player);
                    output.success("§aVanish §bON§a — you are now invisible.");
                }
            });
    }
}
