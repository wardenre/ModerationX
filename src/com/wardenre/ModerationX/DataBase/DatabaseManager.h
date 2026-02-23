#include <memory>
#include <optional>
#include "com/wardenre/ModerationX/DataBase/Wrapper/SQLiteWrapper.h"

namespace com::wardenre::ModerationX::DataBase {

    struct BanInfo {
        int banId;
        std::string name;
        std::string reason;
        std::string admin;
        long long endTime;
    };

    struct WhitelistEntry {
        std::string name;
        std::string xuid;
    };

    class DatabaseManager {
    public:
        static DatabaseManager& getInstance();
        bool init(const std::string& dbPath);

        std::optional<WhitelistEntry> getWhitelistEntry(const std::string& name);
        bool addWhiteList(const std::string& name);
        bool removeWhiteList(const std::string& name);
        bool updateWhitelistXuid(const std::string& name, const std::string& xuid);

        bool isBanned(const std::string& name);
        std::optional<BanInfo> getBanInfo(const std::string& name);
        bool banPlayer(const std::string& name, const std::string& reason, const std::string& admin, long long endTime);
        bool unbanPlayer(const std::string& name);
        int getActiveBanCount();
        std::vector<BanInfo> getAllBans();
        std::vector<WhitelistEntry>  getAllWhitelist();

    private:
        std::unique_ptr<Wrapper::SQLiteWrapper> db;
    };
}