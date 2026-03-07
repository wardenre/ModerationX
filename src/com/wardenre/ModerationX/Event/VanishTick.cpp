#include "com/wardenre/ModerationX/Event/VanishTick.h"
#include "com/wardenre/ModerationX/Manager/VanishManager.h"

#include "ll/api/event/EventBus.h"
#include "ll/api/event/world/LevelTickEvent.h"
#include "ll/api/service/Bedrock.h"

#include "mc/world/level/Level.h"
#include "mc/world/actor/player/Player.h"

namespace com::wardenre::ModerationX::Event {
    using namespace ll::event;
    using namespace Manager;

    void VanishTick::loadEvent(EventBus& eventBus) {
        eventBus.emplaceListener<LevelTickEvent>([](LevelTickEvent&) {
            if (++tickCounter < 5) return;
            tickCounter = 0;

            auto& vm    = VanishManager::getInstance();
            auto  level = ll::service::getLevel();
            if (!level) return;

            level->forEachPlayer([&](Player& player) -> bool {
                if (vm.isVanished(player)) {
                    vm.hidePlayerForAll(player);
                    vm.hideHeldItemForAll(player);
                }
                return true;
            });
        });
    }

} // namespace com::wardenre::ModerationX::Event
