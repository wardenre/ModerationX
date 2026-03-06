#include "mc/world/actor/player/Player.h"

#include <string>
#include <unordered_set>

namespace com::wardenre::ModerationX::Functions {

    class VanishManager {
    public:
        static VanishManager& getInstance();

        void vanish(Player& player);
        void unvanish(Player& player);
        [[nodiscard]] bool isVanished(Player const& player) const;
        [[nodiscard]] bool isVanished(std::string const& name) const;

        void hideHeldItemForAll(Player& player);
        void hidePlayerForAll(Player& player);
        void showPlayerForAll(Player& player);
        void hidePlayerFor(Player& vanished, Player& viewer);
        void refreshForNewPlayer(Player& joiner);

    private:
        std::unordered_set<std::string> mVanished;
    };

}