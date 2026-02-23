#include <ll/api/event/EventBus.h>

namespace com::wardenre::ModerationX::Event {
    using namespace ll::event;
    class PlayerConnect {
        public:
        static void loadEvent(EventBus& EventBus);
    };
}