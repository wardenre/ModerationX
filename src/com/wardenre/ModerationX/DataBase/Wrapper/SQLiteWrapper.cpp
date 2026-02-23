#include "com/wardenre/ModerationX/DataBase/Wrapper/SQLiteWrapper.h"
#include "com/wardenre/ModerationX/ModerationX.h"

namespace com::wardenre::ModerationX::DataBase::Wrapper {
    SQLiteWrapper::SQLiteWrapper(const std::string& dbPath) : db_(nullptr) {
        int rc = sqlite3_open(dbPath.c_str(), &db_);
        if (rc != SQLITE_OK) {
            lastError_ = sqlite3_errmsg(db_);
            ModerationX::getInstance().getLogger().error("Не удалось открыть DB: {}", lastError_);
            sqlite3_close(db_);
            db_ = nullptr;
        }
        if (db_) {
            execute("PRAGMA foreign_keys = ON;");
        }
    }

    SQLiteWrapper::~SQLiteWrapper() {
        if (db_) {
            sqlite3_close(db_);
        }
    }

    bool SQLiteWrapper::execute(const std::string& sql) {
        if (!db_) return false;
        
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg);
        
        if (rc != SQLITE_OK) {
            lastError_ = errMsg ? errMsg : "Unknown error";
            sqlite3_free(errMsg);
            return false;
        }
        
        return true;
    }

    bool SQLiteWrapper::execute(const std::string& sql, const std::vector<std::string>& params) {
        if (!db_) return false;
        
        sqlite3_stmt* stmt = nullptr;
        
        if (!prepareStatement(sql, &stmt)) {
            return false;
        }
    
        if (!bindParameters(stmt, params)) {
            sqlite3_finalize(stmt);
            return false;
        }
        
        int rc = sqlite3_step(stmt);
        bool success = (rc == SQLITE_DONE);
        
        if (!success) {
            lastError_ = sqlite3_errmsg(db_);
        }
        
        sqlite3_finalize(stmt);
        return success;
    }

    std::vector<std::vector<std::string>> SQLiteWrapper::query(const std::string& sql) {
        std::vector<std::vector<std::string>> result;
        
        if (!db_) return result;
        
        sqlite3_stmt* stmt = nullptr;
        
        if (!prepareStatement(sql, &stmt)) {
            return result;
        }
        
        int columnCount = sqlite3_column_count(stmt);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::vector<std::string> row;
            
            for (int i = 0; i < columnCount; i++) {
                const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
                row.push_back(text ? text : "");
            }
            
            result.push_back(row);
        }
        
        sqlite3_finalize(stmt);
        return result;
    }

    std::vector<std::vector<std::string>> SQLiteWrapper::query(
        const std::string& sql, 
        const std::vector<std::string>& params) {
        
        std::vector<std::vector<std::string>> result;
        
        if (!db_) return result;
        
        sqlite3_stmt* stmt = nullptr;
        if (!prepareStatement(sql, &stmt)) {
            return result;
        }
        
        if (!bindParameters(stmt, params)) {
            sqlite3_finalize(stmt);
            return result;
        }
        
        int columnCount = sqlite3_column_count(stmt);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::vector<std::string> row;
            
            for (int i = 0; i < columnCount; i++) {
                const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
                row.push_back(text ? text : "");
            }
            
            result.push_back(row);
        }
        
        sqlite3_finalize(stmt);
        return result;
    }

    bool SQLiteWrapper::beginTransaction() {
        return execute("BEGIN TRANSACTION;");
    }

    bool SQLiteWrapper::commitTransaction() {
        return execute("COMMIT;");
    }

    bool SQLiteWrapper::rollbackTransaction() {
        return execute("ROLLBACK;");
    }

    std::string SQLiteWrapper::getLastError() const {
        return lastError_;
    }

    int64_t SQLiteWrapper::getLastInsertId() const {
        return db_ ? sqlite3_last_insert_rowid(db_) : -1;
    }

    bool SQLiteWrapper::isOpen() const {
        return db_ != nullptr;
    }

    bool SQLiteWrapper::prepareStatement(const std::string& sql, sqlite3_stmt** stmt) {
        int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, stmt, nullptr);
        
        if (rc != SQLITE_OK) {
            lastError_ = sqlite3_errmsg(db_);
            return false;
        }
        
        return true;
    }

    bool SQLiteWrapper::bindParameters(sqlite3_stmt* stmt, const std::vector<std::string>& params) {
        for (int i = 0; i < static_cast<int>(params.size()); i++) {
            int rc = sqlite3_bind_text(stmt, i + 1, params[i].c_str(), -1, SQLITE_TRANSIENT);
            
            if (rc != SQLITE_OK) {
                lastError_ = sqlite3_errmsg(db_);
                return false;
            }
        }
        
        return true;
    }
}