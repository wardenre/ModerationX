#include "com/wardenre/ModerationX/ModerationX.h"
#include "com/wardenre/ModerationX/Config.h"
#include "com/wardenre/ModerationX/Command/Ban.h"
#include "com/wardenre/ModerationX/Command/Unban.h"
#include "com/wardenre/ModerationX/Command/Whitelist.h"
#include "com/wardenre/ModerationX/Command/Vanish.h"
#include "com/wardenre/ModerationX/Event/PlayerConnect.h"
#include "com/wardenre/ModerationX/Event/VanishTick.h"
#include "com/wardenre/ModerationX/DataBase/DatabaseManager.h"
#include "com/wardenre/ModerationX/Hook/VanishHook.h"

#include "ll/api/Config.h"
#include "ll/api/mod/RegisterHelper.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/command/CommandHandle.h"
#include "ll/api/command/CommandRegistrar.h"

namespace com::wardenre::ModerationX {
    using namespace ll::event;
    using namespace ll::command;
    using namespace ll::config;

    using namespace Event;
    using namespace Command;
    using namespace DataBase;
    using namespace Hook;

    ModerationX& ModerationX::getInstance() {
        static ModerationX instance;
        return instance;
    }

    bool ModerationX::load() const {
        const auto& configPath = getSelf().getConfigDir() / "config.json";

        if (!loadConfig(config, configPath)) {
            if (!saveConfig(config, configPath)) {
                getLogger().error("Failed to save config");
                return false;
            }
            getLogger().warn("Config not found — created default config.json");
        }

        std::string dbPath = (getSelf().getDataDir() / config.db.sqlite.dbname += ".db").string();
        if (!DatabaseManager::getInstance().init(dbPath)) {
            getLogger().error("Database initialization failed!");
            return false;
        }

        return true;
    }

    bool ModerationX::enable() const {
        auto& eventBus  = EventBus::getInstance();
        auto& registrar = CommandRegistrar::getInstance(false);

        PlayerConnect::loadEvent(eventBus);
        VanishTick::loadEvent(eventBus);

        Vanish::loadCommand();
        Ban::loadCommand(registrar);
        Unban::loadCommand();
        Whitelist::loadCommand(registrar);

        VanishHook::getInstance();

        return true;
    }

    bool ModerationX::disable() const { return true; }
    bool ModerationX::unload()  const { return true; }

} // namespace com::wardenre::ModerationX

LL_REGISTER_MOD(
    com::wardenre::ModerationX::ModerationX,
    com::wardenre::ModerationX::ModerationX::getInstance()
);
