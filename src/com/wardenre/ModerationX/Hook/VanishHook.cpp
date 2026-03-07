#include "com/wardenre/ModerationX/Hook/VanishHook.h"
#include "com/wardenre/ModerationX/Manager/VanishManager.h"

#include "ll/api/memory/Hook.h"

#include "mc/world/actor/Actor.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/block/actor/ChestBlockActor.h"
#include "mc/server/ServerPlayer.h"
#include "mc/network/Packet.h"

#include <memory>

namespace com::wardenre::ModerationX::Hook {
    using namespace Manager;

    VanishHook& VanishHook::getInstance() {
        static VanishHook instance;
        return instance;
    }

    LL_TYPE_INSTANCE_HOOK(
        ChestOpenHook,
        ll::memory::HookPriority::Normal,
        ChestBlockActor,
        &ChestBlockActor::$startOpen,
        void,
        Actor& actor
    ) {
        if (actor.isPlayer()) {
            auto& player = static_cast<Player&>(actor);
            if (VanishManager::getInstance().isVanished(player)) return;
        }
        origin(actor);
    }

    LL_TYPE_INSTANCE_HOOK(
        ChestCloseHook,
        ll::memory::HookPriority::Normal,
        ChestBlockActor,
        &ChestBlockActor::$stopOpen,
        void,
        Actor& actor
    ) {
        if (actor.isPlayer()) {
            auto& player = static_cast<Player&>(actor);
            if (VanishManager::getInstance().isVanished(player)) return;
        }
        origin(actor);
    }

    LL_TYPE_INSTANCE_HOOK(
        VanishAddActorHook,
        ll::memory::HookPriority::Normal,
        ServerPlayer,
        &ServerPlayer::$tryCreateAddActorPacket,
        std::unique_ptr<Packet>
    ) {
        if (VanishManager::getInstance().isVanished(*this)) return nullptr;
        return origin();
    }

    struct VanishHook::Impl {
        ll::memory::HookRegistrar<
            ChestOpenHook,
            ChestCloseHook,
            VanishAddActorHook
        > hooks;
    };

    VanishHook::VanishHook() : pImpl(std::make_unique<Impl>()) {}
    VanishHook::~VanishHook() = default;

} // namespace com::wardenre::ModerationX::Hook
