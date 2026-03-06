#include "com/wardenre/ModerationX/Hook/VanishContainerHook.h"
#include "com/wardenre/ModerationX/Functions/VanishManager.h"

#include "ll/api/memory/Hook.h"

#include "mc/world/actor/Actor.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/block/actor/ChestBlockActor.h"

#include <memory>

namespace com::wardenre::ModerationX::Hook {
    using namespace Functions;

    VanishContainerHook& VanishContainerHook::getInstance() {
        static VanishContainerHook instance;
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
            if (VanishManager::getInstance().isVanished(player)) {
                return;
            }
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
            if (VanishManager::getInstance().isVanished(player)) {
                return;
            }
        }
        origin(actor);
    }

    struct VanishContainerHook::Impl {
        ll::memory::HookRegistrar<
            ChestOpenHook,
            ChestCloseHook
        > hooks;
    };

    VanishContainerHook::VanishContainerHook() : pImpl(std::make_unique<Impl>()) {}
    VanishContainerHook::~VanishContainerHook() = default;
}
