#include <memory>

namespace com::wardenre::ModerationX::Hook {
    class VanishContainerHook {
        struct Impl;
        std::unique_ptr<Impl> pImpl;

    public:
        VanishContainerHook();
        ~VanishContainerHook();

        static VanishContainerHook& getInstance();
    };
}
