#include <ll/api/command/CommandRegistrar.h>

namespace com::wardenre::ModerationX::Command {
    using namespace ll::command;

    class Vanish {
    public:
        static void loadCommand(CommandRegistrar& registrar);
    };
}
