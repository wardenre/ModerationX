#include "com/wardenre/ModerationX/Event/PlayerConnect.h"
#include "com/wardenre/ModerationX/ModerationX.h"
#include "com/wardenre/ModerationX/Config.h"
#include "com/wardenre/ModerationX/DataBase/DatabaseManager.h"

#include <ll/api/event/EventBus.h>
#include <ll/api/event/player/PlayerConnectEvent.h>
#include <mc/world/actor/player/Player.h>

namespace com::wardenre::ModerationX::Event {
    using namespace ll::event;

    using namespace DataBase;


    void PlayerConnect::loadEvent(EventBus& EventBus) {
        EventBus.emplaceListener<PlayerConnectEvent>([](PlayerConnectEvent& event) {
        Player& player = event.self();
        std::string name = player.getRealName();
        std::string xuid = player.getXuid();
        auto& db = DatabaseManager::getInstance();

        if (db.isBanned(name)) {
            auto info = db.getBanInfo(name);
            player.disconnect("You're banned!\nBan ID: #" + std::to_string(info->banId) + 
                             "\nreason: " + info->reason + 
                             "\nAdmin: " + info->admin);
            return;
        }

        if (config.whitelist.enabled) {
            auto wlData = db.getWhitelistEntry(name);
            
            if (!wlData.has_value()) {
                player.disconnect("You are not on the white list");
                return;
            }

            if (wlData->xuid.empty()) {
                db.updateWhitelistXuid(name, xuid);
                ModerationX::getInstance().getLogger().info("XUID для {} обновлен: {}", name, xuid);
            } 
            else if (config.whitelist.requireXuid && wlData->xuid != xuid) {
                player.disconnect("Security error: XUID does not match the one written in the whitelist!");
                return;
            }
        }
        });
    }
}