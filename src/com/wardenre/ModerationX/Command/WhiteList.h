#include <ll/api/command/CommandRegistrar.h>
#include <string>

namespace com::wardenre::ModerationX::Command {
    using namespace ll::command;
    class Whitelist {
        public:
        static void loadCommand(CommandRegistrar& registrar);
        private:
        enum class WListOperation : int {
            add,
            remove
        };

        struct WListParams {
            WListOperation operation;
            std::string target;
        };
    };
}