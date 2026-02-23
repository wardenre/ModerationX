#include <sqlite3.h>
#include <string>
#include <vector>


namespace com::wardenre::ModerationX::DataBase::Wrapper {
    class SQLiteWrapper {
    public:
        explicit SQLiteWrapper(const std::string& dbPath);
        
        ~SQLiteWrapper();
        
        bool execute(const std::string& sql);
        bool execute(const std::string& sql, const std::vector<std::string>& params);
        std::vector<std::vector<std::string>> query(const std::string& sql);
        std::vector<std::vector<std::string>> query(const std::string& sql, const std::vector<std::string>& params);
        
        bool beginTransaction();
        bool commitTransaction();
        bool rollbackTransaction();
        std::string getLastError() const;
        int64_t getLastInsertId() const;
        
        bool isOpen() const;

    private:
        sqlite3* db_;
        std::string lastError_;
        
        bool prepareStatement(const std::string& sql, sqlite3_stmt** stmt);
        bool bindParameters(sqlite3_stmt* stmt, const std::vector<std::string>& params);
    };
}