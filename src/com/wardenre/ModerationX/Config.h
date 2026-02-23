#include <string>

namespace com::wardenre::ModerationX {
    struct WhiteList {
        bool enabled = true;
        bool requireXuid = true;
    };
    struct Sqlite {
        std::string dbname = "database";
    };

    struct DataBasee {
        Sqlite sqlite;
    };

    struct Config {
        int version = 1;
        bool loginfo = true;
        WhiteList whitelist;
        DataBasee db;
    };

    inline Config config;
}