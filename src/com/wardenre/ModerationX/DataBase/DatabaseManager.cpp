#include "com/wardenre/ModerationX/DataBase/DatabaseManager.h"

#include <ctime>

namespace com::wardenre::ModerationX::DataBase {

    DatabaseManager& DatabaseManager::getInstance() {
        static DatabaseManager instance;
        return instance;
    }

    bool DatabaseManager::init(const std::string& dbPath) {
        db_ = std::make_unique<Wrapper::SQLiteWrapper>(dbPath);
        if (!db_->isOpen()) return false;

        db_->execute(R"(
            CREATE TABLE IF NOT EXISTS bans (
                ban_id     INTEGER PRIMARY KEY AUTOINCREMENT,
                name       TEXT    UNIQUE,
                reason     TEXT,
                admin      TEXT,
                end_time   INTEGER,
                created_at INTEGER
            );
        )");

        db_->execute(R"(
            CREATE TABLE IF NOT EXISTS whitelist (
                name       TEXT PRIMARY KEY,
                xuid       TEXT,
                created_at INTEGER
            );
        )");

        return true;
    }

    std::optional<WhitelistEntry> DatabaseManager::getWhitelistEntry(const std::string& name) {
        auto res = db_->query("SELECT name, xuid FROM whitelist WHERE name = ? COLLATE NOCASE;", {name});
        if (res.empty()) return std::nullopt;
        return WhitelistEntry{res[0][0], res[0][1]};
    }

    bool DatabaseManager::addToWhitelist(const std::string& name) {
        return db_->execute(
            "INSERT OR IGNORE INTO whitelist (name, xuid, created_at) VALUES (?, ?, ?);",
            {name, "", std::to_string(std::time(nullptr))}
        );
    }

    bool DatabaseManager::removeFromWhitelist(const std::string& name) {
        return db_->execute("DELETE FROM whitelist WHERE name = ? COLLATE NOCASE;", {name});
    }

    bool DatabaseManager::updateWhitelistXuid(const std::string& name, const std::string& xuid) {
        return db_->execute("UPDATE whitelist SET xuid = ? WHERE name = ? COLLATE NOCASE;", {xuid, name});
    }

    std::vector<WhitelistEntry> DatabaseManager::getAllWhitelist() {
        std::vector<WhitelistEntry> entries;
        auto res = db_->query("SELECT name, xuid FROM whitelist ORDER BY name;");
        entries.reserve(res.size());
        for (auto& row : res) entries.push_back({row[0], row[1]});
        return entries;
    }

    std::vector<WhitelistEntryFull> DatabaseManager::getAllWhitelistFull() {
        std::vector<WhitelistEntryFull> entries;
        auto res = db_->query("SELECT name, xuid, created_at FROM whitelist ORDER BY name;");
        entries.reserve(res.size());
        for (auto& row : res)
            entries.push_back({row[0], row[1], row[2].empty() ? 0 : std::stoll(row[2])});
        return entries;
    }

    bool DatabaseManager::isBanned(const std::string& name) {
        auto info = getBanInfo(name);
        if (!info) return false;
        if (info->endTime != 0 && info->endTime < std::time(nullptr)) {
            unbanPlayer(name);
            return false;
        }
        return true;
    }

    std::optional<BanInfo> DatabaseManager::getBanInfo(const std::string& name) {
        auto res = db_->query(
            "SELECT ban_id, name, reason, admin, end_time FROM bans WHERE name = ? COLLATE NOCASE;",
            {name}
        );
        if (res.empty()) return std::nullopt;
        return BanInfo{std::stoi(res[0][0]), res[0][1], res[0][2], res[0][3], std::stoll(res[0][4])};
    }

    bool DatabaseManager::banPlayer(
        const std::string& name, const std::string& reason,
        const std::string& admin, long long endTime)
    {
        return db_->execute(
            "INSERT OR REPLACE INTO bans (name, reason, admin, end_time, created_at) VALUES (?, ?, ?, ?, ?);",
            {name, reason, admin, std::to_string(endTime), std::to_string(std::time(nullptr))}
        );
    }

    bool DatabaseManager::unbanPlayer(const std::string& name) {
        return db_->execute("DELETE FROM bans WHERE name = ? COLLATE NOCASE;", {name});
    }

    int DatabaseManager::getActiveBanCount() {
        long long now = std::time(nullptr);
        auto res = db_->query(
            "SELECT COUNT(*) FROM bans WHERE end_time = 0 OR end_time > ?;",
            {std::to_string(now)}
        );
        return res.empty() ? 0 : std::stoi(res[0][0]);
    }

    std::vector<BanInfo> DatabaseManager::getAllBans() {
        std::vector<BanInfo> bans;
        auto res = db_->query("SELECT ban_id, name, reason, admin, end_time FROM bans ORDER BY ban_id DESC;");
        bans.reserve(res.size());
        for (auto& row : res)
            bans.push_back({std::stoi(row[0]), row[1], row[2], row[3], std::stoll(row[4])});
        return bans;
    }

} // namespace com::wardenre::ModerationX::DataBase
