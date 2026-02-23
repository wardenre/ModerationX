#include "com/wardenre/ModerationX/Command/WhiteList.h"
#include "com/wardenre/ModerationX/DataBase/DatabaseManager.h"

#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <ll/api/command/CommandHandle.h>

namespace com::wardenre::ModerationX::Command  {
    void Whitelist::loadCommand(CommandRegistrar& registrar) {
        registrar.tryRegisterEnum<WListOperation>();
        auto& wlist = CommandRegistrar::getInstance(false).getOrCreateCommand("wlist", "add or remove player of hitelist", CommandPermissionLevel::GameDirectors);
        wlist.alias("wl");
        wlist.overload<WListParams>()
            .required("operation")
            .required("target")
            .execute([](CommandOrigin const&, CommandOutput& output, WListParams const& param) {
                auto& db = DataBase::DatabaseManager::getInstance();
                if (param.operation == WListOperation::add) {
                    if (db.getWhitelistEntry(param.target)) {
                        if (!db.addWhiteList(param.target)) {
                            output.error("Error adding to whitelist");
                        } else {
                            output.success("successfully added to the white list");
                        }
                    } else {
                        output.success("The player is already on the white list!");
                    }
                }
                if (param.operation == WListOperation::remove) {
                    if  (db.getWhitelistEntry(param.target)) {
                        if (!db.removeWhiteList(param.target)) {
                            output.error("Error deleting from whitelist");
                        } else {
                            output.success("Successfully removed from the whitelist");
                        }
                    } else {
                        output.success("The player is not on the white list!");
                    }
                }
            });
    }
}