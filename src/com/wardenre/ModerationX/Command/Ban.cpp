#include "com/wardenre/ModerationX/Command/Ban.h"
#include "com/wardenre/ModerationX/DataBase/DatabaseManager.h"
#include "com/wardenre/ModerationX/Utils/TimeFormat.h"

#include "ll/api/command/CommandHandle.h"
#include "ll/api/command/CommandRegistrar.h"

#include "mc/world/actor/player/Player.h"
#include "mc/server/commands/CommandOrigin.h"
#include "mc/server/commands/CommandOutput.h"

namespace com::wardenre::ModerationX::Command {
    using namespace ll::command;
    using namespace DataBase;
    using namespace Utils;

    static std::string getAdminName(CommandOrigin const& origin) {
        auto* entity = origin.getEntity();
        if (!entity)            return "console";
        if (entity->isPlayer()) return static_cast<Player*>(entity)->getRealName();
        return "system";
    }

    void Ban::loadCommand(CommandRegistrar& registrar) {
        registrar.tryRegisterEnum<BanOperation>();

        auto& ban = CommandRegistrar::getInstance(false).getOrCreateCommand(
            "ban", "Ban players", CommandPermissionLevel::GameDirectors
        );

        ban.overload<BanParams>()
            .required("operation")
            .required("player")
            .optional("reason")
            .optional("time")
            .execute([](CommandOrigin const& origin, CommandOutput& output, BanParams const& param) {
                auto& db        = DatabaseManager::getInstance();
                std::string reason    = param.reason.empty() ? "Not specified" : param.reason;
                std::string adminName = getAdminName(origin);
                long long   endTime   = 0;

                switch (param.operation) {
                    case BanOperation::perm: endTime = 0;                                              break;
                    case BanOperation::norm: endTime = minutesToTimestamp(param.time);                 break;
                    case BanOperation::soft: endTime = minutesToTimestamp(std::min(param.time, 1440)); break;
                    case BanOperation::ip:   endTime = 0;                                              break;
                }

                auto results = param.player.results(origin);
                if (results.empty()) { output.error("Player not found"); return; }

                for (Player* pl : results) {
                    std::string name  = pl->getRealName();
                    if (!db.banPlayer(name, reason, adminName, endTime)) continue;

                    auto info  = db.getBanInfo(name);
                    int  banId = info ? info->banId : 0;

                    output.success("Player {} has been banned (ID: #{})", name, banId);
                    pl->disconnect(
                        "You've been banned!\n"
                        "Ban ID: #" + std::to_string(banId) + "\n"
                        "Reason: "  + reason    + "\n"
                        "Admin: "   + adminName + "\n"
                        "Until: "   + formatTime(endTime, "%Y-%m-%d %H:%M:%S")
                    );
                }
            });
    }

} // namespace com::wardenre::ModerationX::Command
