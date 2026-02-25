#include <string>
#include <unordered_set>
#include "mc/world/actor/player/Player.h"

namespace com::wardenre::ModerationX::Functions {

    class VanishManager {
    public:
        static VanishManager& getInstance();

        void vanish(Player& player);
        void unvanish(Player& player);
        bool isVanished(Player const& player) const;
        bool isVanished(std::string const& name) const;

        void hideHeldItemForAll(Player& player);
        void refreshForNewPlayer(Player& joiner);

    private:
        std::unordered_set<std::string> mVanished;
        static constexpr int INVISIBILITY_EFFECT_ID = 14;
    };

}