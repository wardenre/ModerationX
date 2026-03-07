#pragma once

#include "mc/world/actor/player/Player.h"

#include <string>
#include <unordered_set>

namespace com::wardenre::ModerationX::Manager {

    class VanishManager {
    public:
        static VanishManager& getInstance();

        void vanish(Player& player);
        void unvanish(Player& player);

        [[nodiscard]] bool isVanished(Player const& player)    const;
        [[nodiscard]] bool isVanished(std::string const& name) const;

        void hidePlayerForAll(Player& player);
        void showPlayerForAll(Player& player);
        void hidePlayerFor(Player& vanished, Player& viewer);
        void hideHeldItemForAll(Player& player);
        void refreshForNewPlayer(Player& joiner);

    private:
        std::unordered_set<std::string> mVanished_;
    };

} // namespace com::wardenre::ModerationX::Manager
