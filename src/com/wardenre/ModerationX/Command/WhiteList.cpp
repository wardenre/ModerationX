#include "com/wardenre/ModerationX/Command/Whitelist.h"
#include "com/wardenre/ModerationX/DataBase/DatabaseManager.h"
#include "com/wardenre/ModerationX/Utils/TimeFormat.h"

#include "ll/api/command/CommandHandle.h"

#include "mc/server/commands/CommandOrigin.h"
#include "mc/server/commands/CommandOutput.h"

namespace com::wardenre::ModerationX::Command {
    using namespace DataBase;
    using namespace Utils;

    static constexpr int ENTRIES_PER_PAGE = 40;

    void Whitelist::loadCommand(CommandRegistrar& registrar) {
        registrar.tryRegisterEnum<WListOperation>();

        auto& wlist = CommandRegistrar::getInstance(false).getOrCreateCommand(
            "wlist", "Add or remove a player from the whitelist", CommandPermissionLevel::GameDirectors
        );
        wlist.alias("wl");

        wlist.overload<WListParams>()
            .required("operation")
            .required("target")
            .execute([](CommandOrigin const&, CommandOutput& output, WListParams const& param) {
                auto& db = DatabaseManager::getInstance();

                if (param.operation == WListOperation::add) {
                    if (db.getWhitelistEntry(param.target)) {
                        output.error("The player is already on the whitelist!");
                        return;
                    }
                    db.addToWhitelist(param.target)
                        ? output.success("Successfully added to the whitelist")
                        : output.error("Error adding to whitelist");
                    return;
                }

                if (param.operation == WListOperation::remove) {
                    if (!db.getWhitelistEntry(param.target)) {
                        output.error("The player is not on the whitelist!");
                        return;
                    }
                    db.removeFromWhitelist(param.target)
                        ? output.success("Successfully removed from the whitelist")
                        : output.error("Error deleting from whitelist");
                    return;
                }
            });

        wlist.overload<WListPageParams>()
            .text("list")
            .optional("page")
            .execute([](CommandOrigin const&, CommandOutput& output, WListPageParams const& param) {
                auto& db  = DatabaseManager::getInstance();
                auto  all = db.getAllWhitelistFull();

                if (all.empty()) { output.error("Whitelist is empty!"); return; }

                int totalPages = (static_cast<int>(all.size()) + ENTRIES_PER_PAGE - 1) / ENTRIES_PER_PAGE;
                int page       = param.page > 0 ? param.page : 1;
                if (page > totalPages) page = totalPages;

                int startIdx = (page - 1) * ENTRIES_PER_PAGE;
                int endIdx   = std::min(startIdx + ENTRIES_PER_PAGE, static_cast<int>(all.size()));

                output.success(
                    "§e===== Whitelist §f[§a" + std::to_string(page) +
                    "§f/§a" + std::to_string(totalPages) +
                    "§f] §e(§f" + std::to_string(all.size()) + " total§e) ====="
                );
                output.success("§7------------------------------------");
                output.success("§b Name          §f| §b XUID            §f| §b Created");
                output.success("§7------------------------------------");

                for (int i = startIdx; i < endIdx; i++) {
                    auto& e = all[i];
                    output.success(
                        "§a " + e.name +
                        " §f| " + (e.xuid.empty() ? "§cNULL" : "§f" + e.xuid) +
                        " §f| §7" + formatTime(e.createdAt)
                    );
                }

                output.success("§7------------------------------------");
                if (totalPages > 1)
                    output.success("§7Use §f/wl list <page> §7to navigate");
            });
    }

} // namespace com::wardenre::ModerationX::Command
