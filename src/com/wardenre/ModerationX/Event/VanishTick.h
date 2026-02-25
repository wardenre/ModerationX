#include <ll/api/event/EventBus.h>

namespace com::wardenre::ModerationX::Event {
    using namespace ll::event;

    class VanishTick {
    public:
        static void loadEvent(EventBus& eventBus);
    private:
        static int tickCounter = 0;
    };
}
