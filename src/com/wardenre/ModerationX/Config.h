#pragma once

#include <string>

namespace com::wardenre::ModerationX {

    struct WhitelistConfig {
        bool enabled     = true;
        bool requireXuid = true;
    };

    struct SqliteConfig {
        std::string dbname = "database";
    };

    struct DatabaseConfig {
        SqliteConfig sqlite;
    };

    struct Config {
        int             version = 1;
        bool            loginfo = true;
        WhitelistConfig whitelist;
        DatabaseConfig  db;
    };

    inline Config config;

} // namespace com::wardenre::ModerationX
