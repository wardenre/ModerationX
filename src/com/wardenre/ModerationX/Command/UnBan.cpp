#include "com/wardenre/ModerationX/Command/Unban.h"
#include "com/wardenre/ModerationX/DataBase/DatabaseManager.h"

#include "ll/api/command/CommandHandle.h"
#include "ll/api/command/CommandRegistrar.h"

#include "mc/server/commands/CommandOrigin.h"
#include "mc/server/commands/CommandOutput.h"

namespace com::wardenre::ModerationX::Command {
    using namespace ll::command;
    using namespace DataBase;

    void Unban::loadCommand() {
        auto& unban = CommandRegistrar::getInstance(false).getOrCreateCommand(
            "unban", "Unban players", CommandPermissionLevel::GameDirectors
        );

        unban.overload<UnbanParams>()
            .required("player")
            .execute([](CommandOrigin const&, CommandOutput& output, UnbanParams const& param) {
                auto& db = DatabaseManager::getInstance();
                if (!db.isBanned(param.player)) {
                    output.error("Player {} is not banned!", param.player);
                } else {
                    db.unbanPlayer(param.player);
                    output.success("Player {} has been unbanned!", param.player);
                }
            });
    }

} // namespace com::wardenre::ModerationX::Command
