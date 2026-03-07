#pragma once

#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace com::wardenre::ModerationX::Utils {

    inline std::string formatTime(long long timestamp, const char* fmt = "%d.%m.%Y %H:%M") {
        if (timestamp == 0) return "N/A";
        std::time_t t = static_cast<std::time_t>(timestamp);
        std::tm tm{};
#ifdef _WIN32
        localtime_s(&tm, &t);
#else
        localtime_r(&t, &tm);
#endif
        std::ostringstream oss;
        oss << std::put_time(&tm, fmt);
        return oss.str();
    }

    inline long long minutesToTimestamp(int minutes) {
        if (minutes <= 0) return 0;
        return static_cast<long long>(std::time(nullptr)) + (minutes * 60LL);
    }

} // namespace com::wardenre::ModerationX::Utils
