#include <string>

namespace com::wardenre::ModerationX::Command {
    class UnBan {
        public:
        static void loadCommand();
        private:
        struct UnBanParams {
            std::string player;
        };
    };
}