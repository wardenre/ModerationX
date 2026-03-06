#include "com/wardenre/ModerationX/ModerationX.h"
#include "com/wardenre/ModerationX/Config.h" 
#include "com/wardenre/ModerationX/Command/Ban.h"
#include "com/wardenre/ModerationX/Command/UnBan.h"
#include "com/wardenre/ModerationX/Command/WhiteList.h"
#include "com/wardenre/ModerationX/Event/PlayerConnect.h"
#include "com/wardenre/ModerationX/DataBase/DatabaseManager.h"
#include "com/wardenre/ModerationX/Command/Vanish.h"
#include "com/wardenre/ModerationX/Event/VanishTick.h"
#include "com/wardenre/ModerationX/Hook/VanishContainerHook.h"

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

    auto& registrar = CommandRegistrar::getInstance(false);
    auto& eventbus = EventBus::getInstance();

    ModerationX& ModerationX::getInstance() {
        static ModerationX instance;
        return instance;
    }

    bool ModerationX::load() const {
        if (config.loginfo) {
        }

        const auto& ConfigPath = getSelf().getConfigDir() / "config.json";
        if (!loadConfig(config, ConfigPath)) {
            if (!saveConfig(config, ConfigPath)) {
                getLogger().error("Failed to save config");
                return false;
            }
            getLogger().error("Failed to load config");
            return false;
        }

        std::string dbPath = (getSelf().getDataDir() / config.db.sqlite.dbname += ".db").string();
        if (!DatabaseManager::getInstance().init(dbPath)) {
            getLogger().error("Database initialization error!");
            return false;
        }
        return true;
    }

    bool ModerationX::enable() const {
        PlayerConnect::loadEvent(eventbus);
        VanishTick::loadEvent(eventbus); 

        Vanish::loadCommand();
        Ban::loadCommand(registrar);
        UnBan::loadCommand();
        Whitelist::loadCommand(registrar);

        VanishContainerHook::getInstance();
        
        return true;
    }

    bool ModerationX::disable() const {
        return true;
    }

    bool ModerationX::unload() const {
        return true;
    }

}


LL_REGISTER_MOD(com::wardenre::ModerationX::ModerationX, com::wardenre::ModerationX::ModerationX::getInstance());