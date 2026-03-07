#include "com/wardenre/ModerationX/Manager/VanishManager.h"
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

namespace com::wardenre::ModerationX::Manager {

    VanishManager& VanishManager::getInstance() {
        static VanishManager instance;
        return instance;
    }

    bool VanishManager::isVanished(Player const& player) const {
        return mVanished_.count(player.getRealName()) > 0;
    }

    bool VanishManager::isVanished(std::string const& name) const {
        return mVanished_.count(name) > 0;
    }

    void VanishManager::vanish(Player& player) {
        std::string name = player.getRealName();
        mVanished_.insert(name);
        ModerationX::getInstance().getLogger().info("{} is now VANISHED", name);
        hidePlayerForAll(player);
    }

    void VanishManager::unvanish(Player& player) {
        std::string name = player.getRealName();
        mVanished_.erase(name);
        ModerationX::getInstance().getLogger().info("{} is now VISIBLE", name);
        showPlayerForAll(player);
    }

    static void sendRemoveActor(Player& target, Player& viewer) {
        RemoveActorPacket pkt;
        ll::memory::dAccess<ActorUniqueID>(&pkt, 48) = target.getOrCreateUniqueID();
        viewer.sendNetworkPacket(pkt);
    }

    void VanishManager::hidePlayerForAll(Player& player) {
        player.getLevel().forEachPlayer([&](Player& other) -> bool {
            if (&other != &player) sendRemoveActor(player, other);
            return true;
        });
    }

    void VanishManager::showPlayerForAll(Player& player) {
        auto addPkt = player.tryCreateAddActorPacket();
        if (!addPkt) return;
        player.getLevel().forEachPlayer([&](Player& other) -> bool {
            if (&other != &player) other.sendNetworkPacket(*addPkt);
            return true;
        });
    }

    void VanishManager::hidePlayerFor(Player& vanished, Player& viewer) {
        sendRemoveActor(vanished, viewer);
    }

    void VanishManager::hideHeldItemForAll(Player& player) {
        ItemStack          emptyItem;
        MobEquipmentPacket pkt(player.getRuntimeID(), emptyItem, 0, 0, ContainerID::Inventory);
        player.getLevel().forEachPlayer([&](Player& other) -> bool {
            if (&other != &player) other.sendNetworkPacket(pkt);
            return true;
        });
    }

    void VanishManager::refreshForNewPlayer(Player& joiner) {
        joiner.getLevel().forEachPlayer([&](Player& other) -> bool {
            if (&other != &joiner && isVanished(other)) hidePlayerFor(other, joiner);
            return true;
        });
    }

} // namespace com::wardenre::ModerationX::Manager
