#include "com/wardenre/ModerationX/Functions/VanishManager.h"
#include "com/wardenre/ModerationX/ModerationX.h"

#include "mc/world/level/Level.h"
#include "mc/world/effect/MobEffectInstance.h"
#include "mc/network/packet/MobEquipmentPacket.h"
#include "mc/world/item/ItemStack.h"
#include "mc/world/actor/player/Player.h"

namespace com::wardenre::ModerationX::Functions {

    VanishManager& VanishManager::getInstance() {
        static VanishManager instance;
        return instance;
    }

    bool VanishManager::isVanished(Player const& player) const {
        return mVanished.count(player.getRealName()) > 0;
    }

    bool VanishManager::isVanished(std::string const& name) const {
        return mVanished.count(name) > 0;
    }

    void VanishManager::vanish(Player& player) {
        std::string name = player.getRealName();
        mVanished.insert(name);

        auto& logger = ModerationX::getInstance().getLogger();
        logger.info("{} is now VANISHED", name);

        MobEffectInstance invisEffect(static_cast<uint>(INVISIBILITY_EFFECT_ID));
        player.addEffect(invisEffect);
        player.setInvisible(true);
        hideHeldItemForAll(player);
    }

    void VanishManager::unvanish(Player& player) {
        std::string name = player.getRealName();
        mVanished.erase(name);

        auto& logger = ModerationX::getInstance().getLogger();
        logger.info("{} is now VISIBLE", name);
        player.removeEffect(INVISIBILITY_EFFECT_ID);
        player.setInvisible(false);
    }

    void VanishManager::hideHeldItemForAll(Player& player) {
        Level& level = player.getLevel();

        ItemStack emptyItem;

        MobEquipmentPacket pkt(
            player.getRuntimeID(),
            emptyItem,
            0,
            0,
            ContainerID::Inventory
        );

        level.forEachPlayer([&](Player& other) -> bool {
            if (&other != &player) {
                other.sendNetworkPacket(pkt);
            }
            return true;
        });
    }

    void VanishManager::refreshForNewPlayer(Player& joiner) {
        Level& level = joiner.getLevel();

        level.forEachPlayer([&](Player& other) -> bool {
            if (&other != &joiner && isVanished(other)) {
                hideHeldItemForAll(other);
            }
            return true;
        });
    }
}
