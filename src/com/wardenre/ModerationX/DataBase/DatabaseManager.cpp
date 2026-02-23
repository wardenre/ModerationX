#include "com/wardenre/ModerationX/DataBase/DatabaseManager.h"
#include <ctime>

namespace com::wardenre::ModerationX::DataBase {

    DatabaseManager& DatabaseManager::getInstance() {
        static DatabaseManager instance;
        return instance;
    }

    bool DatabaseManager::init(const std::string& dbPath) {
        db = std::make_unique<Wrapper::SQLiteWrapper>(dbPath);
        if (!db->isOpen()) return false;

        db->execute(R"(
            CREATE TABLE IF NOT EXISTS bans (
                ban_id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT UNIQUE, 
                reason TEXT, 
                admin TEXT, 
                end_time INTEGER, 
                created_at INTEGER
            );
        )");

        db->execute(R"(
            CREATE TABLE IF NOT EXISTS whitelist (
                name TEXT PRIMARY KEY, 
                xuid TEXT, 
                created_at INTEGER
            );
        )");
        
        return true;
    }


    std::optional<WhitelistEntry> DatabaseManager::getWhitelistEntry(const std::string& name) {
        auto res = db->query("SELECT name, xuid FROM whitelist WHERE name = ? COLLATE NOCASE;", {name});
        if (res.empty()) return std::nullopt;
        return WhitelistEntry{res[0][0], res[0][1]};
    }

    bool DatabaseManager::addWhiteList(const std::string& name) {
        return db->execute("INSERT OR IGNORE INTO whitelist (name, xuid, created_at) VALUES (?, ?, ?);", 
            {name, "", std::to_string(std::time(nullptr))});
    }

    bool DatabaseManager::removeWhiteList(const std::string& name) {
        return db->execute("DELETE FROM whitelist WHERE name = ? COLLATE NOCASE;", {name});
    }

    bool DatabaseManager::updateWhitelistXuid(const std::string& name, const std::string& xuid) {
        return db->execute("UPDATE whitelist SET xuid = ? WHERE name = ? COLLATE NOCASE;", {xuid, name});
    }

    std::vector<WhitelistEntry> DatabaseManager::getAllWhitelist() {
        std::vector<WhitelistEntry> entries;
        auto res = db->query("SELECT name, xuid FROM whitelist ORDER BY name;");
        
        for (auto& row : res) {
            entries.push_back(WhitelistEntry{row[0], row[1]});
        }
        
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
        auto res = db->query("SELECT ban_id, name, reason, admin, end_time FROM bans WHERE name = ? COLLATE NOCASE;", {name});
        if (res.empty()) return std::nullopt;

        BanInfo info;
        info.banId = std::stoi(res[0][0]);
        info.name = res[0][1];
        info.reason = res[0][2];
        info.admin = res[0][3];
        info.endTime = std::stoll(res[0][4]);
        return info;
    }

    bool DatabaseManager::banPlayer(const std::string& name, const std::string& reason, const std::string& admin, long long endTime) {
        return db->execute("INSERT OR REPLACE INTO bans (name, reason, admin, end_time, created_at) VALUES (?, ?, ?, ?, ?);", 
            {name, reason, admin, std::to_string(endTime), std::to_string(std::time(nullptr))});
    }

    bool DatabaseManager::unbanPlayer(const std::string& name) {
        return db->execute("DELETE FROM bans WHERE name = ? COLLATE NOCASE;", {name});
    }

    std::vector<BanInfo> DatabaseManager::getAllBans() {
        std::vector<BanInfo> bans;
        auto res = db->query("SELECT ban_id, name, reason, admin, end_time FROM bans ORDER BY ban_id DESC;");
        
        for (auto& row : res) {
            BanInfo info;
            info.banId = std::stoi(row[0]);
            info.name = row[1];
            info.reason = row[2];
            info.admin = row[3];
            info.endTime = std::stoll(row[4]);
            bans.push_back(info);
        }
        
        return bans;
    }

    int DatabaseManager::getActiveBanCount() {
        long long now = std::time(nullptr);
        int count = 0;
        
        auto res = db->query("SELECT end_time FROM bans;");
        for (auto& row : res) {
            long long endTime = std::stoll(row[0]);
            if (endTime == 0 || endTime > now) {
                count++;
            }
        }
        
        return count;
    }
}