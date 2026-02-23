#include "com/wardenre/ModerationX/Command/UnBan.h"
#include "com/wardenre/ModerationX/DataBase/DatabaseManager.h"

#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <ll/api/command/CommandHandle.h>
#include <ll/api/command/CommandRegistrar.h>

namespace com::wardenre::ModerationX::Command {
    using namespace ll::command;
    using namespace DataBase;
    void UnBan::loadCommand() {
        auto& unban = ll::command::CommandRegistrar::getInstance(false).getOrCreateCommand("unban", "unbaned players", CommandPermissionLevel::GameDirectors);
        unban.overload<UnBanParams>().required("player").execute([](CommandOrigin const&, CommandOutput& output, UnBanParams const& param) {
            auto& database = DatabaseManager::getInstance();
            if (!database.isBanned(param.player)) {
                output.success("Player {} is not in the ban!", param.player);
            } else {
                database.unbanPlayer(param.player);
                output.success("Player {} has been unbanned!", param.player);
            }
        });
    }
}