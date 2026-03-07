#include "com/wardenre/ModerationX/Event/PlayerConnect.h"
#include "com/wardenre/ModerationX/ModerationX.h"
#include "com/wardenre/ModerationX/Config.h"
#include "com/wardenre/ModerationX/DataBase/DatabaseManager.h"
#include "com/wardenre/ModerationX/Manager/VanishManager.h"

#include "ll/api/event/EventBus.h"
#include "ll/api/event/player/PlayerConnectEvent.h"
#include "mc/world/actor/player/Player.h"

namespace com::wardenre::ModerationX::Event {
    using namespace ll::event;
    using namespace DataBase;

    void PlayerConnect::loadEvent(EventBus& eventBus) {
        eventBus.emplaceListener<PlayerConnectEvent>([](PlayerConnectEvent& event) {
            Player&     player = event.self();
            std::string name   = player.getRealName();
            std::string xuid   = player.getXuid();
            auto& db           = DatabaseManager::getInstance();

            if (db.isBanned(name)) {
                auto info = db.getBanInfo(name);
                player.disconnect(
                    "You're banned!\n"
                    "Ban ID: #" + std::to_string(info->banId) +
                    "\nReason: "  + info->reason +
                    "\nAdmin: "   + info->admin
                );
                return;
            }

            if (config.whitelist.enabled) {
                auto wlData = db.getWhitelistEntry(name);

                if (!wlData.has_value()) {
                    player.disconnect("You are not on the whitelist");
                    return;
                }

                if (wlData->xuid.empty()) {
                    db.updateWhitelistXuid(name, xuid);
                    ModerationX::getInstance().getLogger().info("XUID for {} updated: {}", name, xuid);
                } else if (config.whitelist.requireXuid && wlData->xuid != xuid) {
                    player.disconnect("Security error: XUID mismatch!");
                    return;
                }
            }

            Manager::VanishManager::getInstance().refreshForNewPlayer(player);
        });
    }

} // namespace com::wardenre::ModerationX::Event
