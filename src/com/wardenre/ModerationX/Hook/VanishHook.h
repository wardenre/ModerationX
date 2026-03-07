#pragma once

#include <memory>

namespace com::wardenre::ModerationX::Hook {

    class VanishHook {
    public:
        VanishHook();
        ~VanishHook();

        static VanishHook& getInstance();

    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };

} // namespace com::wardenre::ModerationX::Hook
