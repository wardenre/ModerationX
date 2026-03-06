#include "com/wardenre/ModerationX/Functions/VanishManager.h"
#include "com/wardenre/ModerationX/ModerationX.h"

#include "mc/world/level/Level.h"
#include "mc/network/packet/RemoveActorPacket.h"
#include "mc/network/packet/MobEquipmentPacket.h"
#include "mc/network/Packet.h"
#include "mc/world/item/ItemStack.h"
#include "mc/world/actor/Actor.h"
#include "mc/world/actor/player/Player.h"
#include "mc/legacy/ActorUniqueID.h"
#include "ll/api/memory/Memory.h"

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

        hidePlayerForAll(player);
    }

    void VanishManager::unvanish(Player& player) {
        std::string name = player.getRealName();
        mVanished.erase(name);

        auto& logger = ModerationX::getInstance().getLogger();
        logger.info("{} is now VISIBLE", name);

        showPlayerForAll(player);
    }

    static void sendRemoveActor(Player& target, Player& viewer) {
        RemoveActorPacket pkt;
        auto id = target.getOrCreateUniqueID();
        ll::memory::dAccess<ActorUniqueID>(&pkt, 48) = id;
        viewer.sendNetworkPacket(pkt);
    }

    void VanishManager::hidePlayerForAll(Player& player) {
        Level& level = player.getLevel();

        level.forEachPlayer([&](Player& other) -> bool {
            if (&other != &player) {
                sendRemoveActor(player, other);
            }
            return true;
        });
    }

    void VanishManager::showPlayerForAll(Player& player) {
        Level& level = player.getLevel();

        auto addPkt = player.tryCreateAddActorPacket();
        if (addPkt) {
            level.forEachPlayer([&](Player& other) -> bool {
                if (&other != &player) {
                    other.sendNetworkPacket(*addPkt);
                }
                return true;
            });
        }
    }

    void VanishManager::hidePlayerFor(Player& vanished, Player& viewer) {
        sendRemoveActor(vanished, viewer);
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
                hidePlayerFor(other, joiner);
            }
            return true;
        });
    }
}
