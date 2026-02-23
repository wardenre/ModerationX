#pragma once

#include <ll/api/io/Logger.h>
#include <ll/api/mod/NativeMod.h>

namespace com::wardenre::ModerationX {

class ModerationX {

public:
    static ModerationX& getInstance();

    ModerationX() : mSelf(*ll::mod::NativeMod::current()) {}

    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return mSelf; }
    [[nodiscard]] ll::io::Logger& getLogger() const { return mSelf.getLogger(); }

    bool load();
    bool enable();
    bool disable();
    bool unload();

private:
    ll::mod::NativeMod& mSelf;
};

}
