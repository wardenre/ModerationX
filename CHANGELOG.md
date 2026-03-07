# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

# v0.2.0-rc.1 - 2026-03-07

### Added
- Vanish system — players with the `vanish` permission can toggle invisibility (`/vanish`, alias `/v`)
- Vanish hooks — chest open/close suppression and `AddActorPacket` blocking for vanished players
- Database viewer — `/wl list [page]` command for paginated whitelist inspection

### Fixed
- Database record insertion now uses `INSERT OR REPLACE` to correctly handle duplicate entries
- Vanish system no longer briefly reveals the player on tick; held item is now also hidden for all viewers
- New players joining now correctly have all currently vanished players hidden from them

### Changed
- Restructured plugin hierarchy: `Functions/` renamed to `Manager/`, added `Utils/` for shared utilities (`TimeFormat.h`)
- `WhiteList` → `Whitelist`, `UnBan` → `Unban` for naming consistency across files and classes
- `enum BanOperation` promoted to `enum class` for type safety
- `getActiveBanCount()` filter moved from C++ to SQL

*Released by [@wardenre](https://github.com/wardenre)*

---

# v0.1.0 - 2026-02-23

- Initial release by [@wardenre](https://github.com/wardenre)
