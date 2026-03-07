#include "com/wardenre/ModerationX/Command/WhiteList.h"
#include "com/wardenre/ModerationX/DataBase/DatabaseManager.h"

#include "ll/api/command/CommandHandle.h"

#include "mc/server/commands/CommandOrigin.h"
#include "mc/server/commands/CommandOutput.h"

#include <ctime>
#include <iomanip>
#include <sstream>

namespace com::wardenre::ModerationX::Command {

    static std::string formatTime(long long timestamp) {
        if (timestamp == 0) return "N/A";
        std::time_t t = static_cast<std::time_t>(timestamp);
        std::tm tm{};
#ifdef _WIN32
        localtime_s(&tm, &t);
#else
        localtime_r(&t, &tm);
#endif
        std::ostringstream oss;
        oss << std::put_time(&tm, "%d.%m.%Y %H:%M");
        return oss.str();
    }

    static constexpr int ENTRIES_PER_PAGE = 40;

    void Whitelist::loadCommand(CommandRegistrar& registrar) {
        registrar.tryRegisterEnum<WListOperation>();
        auto& wlist = CommandRegistrar::getInstance(false).getOrCreateCommand(
            "wlist",
            "add or remove player of whitelist",
            CommandPermissionLevel::GameDirectors);
        wlist.alias("wl");

        wlist.overload<WListParams>()
            .required("operation")
            .required("target")
            .execute([](CommandOrigin const&, CommandOutput& output, WListParams const& param) {
                auto& db = DataBase::DatabaseManager::getInstance();

                if (param.operation == WListOperation::add) {
                    if (db.getWhitelistEntry(param.target)) {
                        output.error("The player is already on the white list!");
                        return;
                    }
                    if (db.addWhiteList(param.target)) {
                        output.success("Successfully added to the white list");
                    } else {
                        output.error("Error adding to whitelist");
                    }
                    return;
                }

                if (param.operation == WListOperation::remove) {
                    if (!db.getWhitelistEntry(param.target)) {
                        output.error("The player is not on the white list!");
                        return;
                    }
                    if (db.removeWhiteList(param.target)) {\
                        output.success("Successfully removed from the whitelist");
                    } else {
                        output.error("Error deleting from whitelist");
                    }
                    return;
                }
            });

        wlist.overload<WListListParams>()
            .text("list")
            .optional("page")
            .execute([](CommandOrigin const&, CommandOutput& output, WListListParams const& param) {

                auto& db = DataBase::DatabaseManager::getInstance();
                auto all = db.getAllWhitelistFull();

                if (all.empty()) {
                    output.error("Whitelist is empty!");
                    return;
                }

                int totalPages = (static_cast<int>(all.size()) + ENTRIES_PER_PAGE - 1) / ENTRIES_PER_PAGE;
                int page = param.page > 0 ? param.page : 1;
                if (page > totalPages) page = totalPages;

                int startIdx = (page - 1) * ENTRIES_PER_PAGE;
                int endIdx = std::min(startIdx + ENTRIES_PER_PAGE, static_cast<int>(all.size()));

                std::string header = 
                    "§e===== Whitelist §f[§a" + std::to_string(page) 
                    + "§f/§a" + std::to_string(totalPages) 
                    + "§f] §e(§f" + std::to_string(all.size()) 
                    + " total§e) =====";
                output.success(header);

                output.success("§7------------------------------------");
                output.success("§b Name          §f| §b XUID            §f| §b Created");
                output.success("§7------------------------------------");

                for (int i = startIdx; i < endIdx; i++) {
                    auto& entry = all[i];
                    std::string xuid = entry.xuid.empty() ? "§cNULL" : "§f" + entry.xuid;
                    std::string created = formatTime(entry.createdAt);

                    std::string line = "§a " + entry.name 
                        + " §f| " + xuid 
                        + " §f| §7" + created;
                    output.success(line);
                }

                output.success("§7------------------------------------");
                if (totalPages > 1) {
                    output.success("§7Use §f/wl list <page> §7to navigate");
                }
            });
    }
}
