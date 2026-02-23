#include "com/wardenre/ModerationX/Command/Ban.h"
#include "com/wardenre/ModerationX/DataBase/DatabaseManager.h"
#include "com/wardenre/ModerationX/ModerationX.h"

#include <mc/world/actor/player/Player.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <ll/api/command/CommandHandle.h>
#include <ll/api/command/CommandRegistrar.h>
#include <ll/api/service/PlayerInfo.h>
#include <ctime>
#include <iomanip>

namespace com::wardenre::ModerationX::Command {
    using namespace ll::command;

    static std::string formatTime(long long timestamp) {
        if (timestamp == 0) return "Forever";
        std::time_t t = static_cast<std::time_t>(timestamp);
        std::tm tm;
        #ifdef _WIN32
        localtime_s(&tm, &t);
        #endif
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    static long long parseTime(int minutes) {
        if (minutes <= 0) return 0;
        return std::time(nullptr) + (minutes * 60LL);
    }

    static std::string getAdminName(CommandOrigin const& origin) {
        auto* entity = origin.getEntity();
        if (!entity) return "console";
        if (entity->isPlayer()) return static_cast<Player*>(entity)->getRealName();
        return "system";
    }

    void Ban::loadCommand(CommandRegistrar &registrar) {
        registrar.tryRegisterEnum<BanOperation>();
        auto& ban = CommandRegistrar::getInstance(false).getOrCreateCommand("ban", "banned players", CommandPermissionLevel::GameDirectors);
        
        ban.overload<BanParams>()
            .required("operation")
            .required("player")
            .optional("reason")
            .optional("time")
            .execute([](CommandOrigin const& origin, CommandOutput& output, BanParams const& param) {
                
            auto& db = DataBase::DatabaseManager::getInstance();
            
            std::string reason = param.reason.empty() ? "Not specified" : param.reason;
            std::string adminName = getAdminName(origin);
            long long endTime = 0;

            switch (param.operation) {
                case BanOperation::perm: endTime = 0; break;
                case BanOperation::norm: endTime = parseTime(param.time); break;
                case BanOperation::soft: endTime = parseTime(std::min(param.time, 1440)); break;
                case BanOperation::ip:   endTime = 0; break;
            }

            auto results = param.player.results(origin);
            if (!results.empty()) {
                for (Player* pl : results) {
                    std::string name = pl->getRealName();
                    
                    if (db.banPlayer(name, reason, adminName, endTime)) {
                        auto info = db.getBanInfo(name);
                        output.success("Player {} has been banned (ID: #{})", name, info ? info->banId : 0);
                        
                        pl->disconnect(
                            "You've been banned!\n"
                            "Ban ID: #" + std::to_string(info ? info->banId : 0) + "\n"
                            "Reason: " + reason + "\n"
                            "Admin: " + adminName + "\n"
                            "Before: " + formatTime(endTime)
                        );
                    }
                }
            } 
            else {
                output.error("Player not found");
            }
        });
    }
}