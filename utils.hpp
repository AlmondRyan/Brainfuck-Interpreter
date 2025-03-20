#include <string>
#include <utility>
#include <regex>

#ifdef _WIN32

#include <windows.h>
#include <VersionHelpers.h>

#elif defined(__linux__)
#include <fstream>
#include <sys/utsname.h>
#elif defined(__APPLE__)
#include <sys/sysctl.h>
#endif

namespace YAOBI {
    struct Version {
        int major, minor, patch;
        std::string preRelease;
        std::string buildMetadata;

        explicit Version(int maj = 0, int min = 0, int pat = 0, std::string pre = "", std::string build = "")
                : major(maj), minor(min), patch(pat), preRelease(std::move(pre)), buildMetadata(std::move(build)) {}

        bool operator<(const Version &other) const {
            if (major != other.major) return major < other.major;
            if (minor != other.minor) return minor < other.minor;
            if (patch != other.patch) return patch < other.patch;
            return preRelease < other.preRelease;
        }

        bool operator==(const Version &other) const {
            return major == other.major && minor == other.minor && patch == other.patch &&
                   preRelease == other.preRelease && buildMetadata == other.buildMetadata;
        }
    };

    Version parseVersionStr(const std::string &version) {
        /*
         * Conventions:
         * Splitters:
         *     "." - Major, Minor, Patch. Like 1.0.0, 1.3.15
         *     "-" - Front of Pre-Release Identifier. Like 1.0.0-alpha
         *     "+" - Front of Build Meta Info. Like 1.0.0+build130
         * Type:
         *     [Major].[Minor].[Patch][-<PreReleaseVersion>][+<BuildMetaData>]
         *     - MAJOR version when you make incompatible API changes
         *     - MINOR version when you add functionality in a backward compatible manner
         *     - PATCH version when you make backward compatible bug fixes
         *
         *     Reference: https://semver.org
         */
        std::regex re(R"(^(\d+)\.(\d+)\.(\d+)(?:-([0-9A-Za-z-]+))?(?:\+([0-9A-Za-z-]+))?$)");
        std::smatch match;
        Version ver;

        if (std::regex_match(version, match, re)) {
            ver.major = std::stoi(match[1].str());
            ver.minor = std::stoi(match[2].str());
            ver.patch = std::stoi(match[3].str());
            if (match[4].matched) ver.preRelease = match[4];
            if (match[5].matched) ver.buildMetadata = match[5];
        }
        return ver;
    }

#ifdef _WIN32

    typedef NTSTATUS(WINAPI *RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

    std::string GetSysVersion() {
        std::string version;

        // 使用 RtlGetVersion 获取真实版本号
        HMODULE hMod = GetModuleHandleW(L"ntdll.dll");
        if (hMod) {
            auto RtlGetVersion = (RtlGetVersionPtr) GetProcAddress(hMod, "RtlGetVersion");
            if (RtlGetVersion) {
                OSVERSIONINFOEXW osvi = {sizeof(osvi)};
                RtlGetVersion((PRTL_OSVERSIONINFOW) &osvi);

                if (osvi.dwMajorVersion == 10) {
                    if (osvi.dwBuildNumber >= 22000) {
                        version = "Windows 11";
                    } else {
                        version = "Windows 10";
                    }
                } else if (osvi.dwMajorVersion == 6) {
                    switch (osvi.dwMinorVersion) {
                        case 3:
                            version = "Windows 8.1";
                            break;
                        case 2:
                            version = "Windows 8";
                            break;
                        case 1:
                            version = "Windows 7";
                            break;
                        default:
                            version = "Windows Vista";
                            break;
                    }
                } else {
                    version = "Windows " + std::to_string(osvi.dwMajorVersion) + "."
                              + std::to_string(osvi.dwMinorVersion);
                }
            }
        }

        if (version.empty()) {
            version = "Unknown Windows Version";
        }
        return version;
    }

    int GetSysBit() {
        SYSTEM_INFO sysInfo;
        GetNativeSystemInfo(&sysInfo);

        switch (sysInfo.wProcessorArchitecture) {
            case PROCESSOR_ARCHITECTURE_AMD64:
            case PROCESSOR_ARCHITECTURE_ARM64:
                return 64;
            case PROCESSOR_ARCHITECTURE_INTEL:
            case PROCESSOR_ARCHITECTURE_ARM:
                return 32;
            default:
                return 0;
        }
    }

#elif defined(__linux__)
    std::string GetSysVersion() {
        std::string version = "Unknown Linux Distribution";
        std::ifstream osRelease("/etc/os-release");
        std::string line;

        if (osRelease.is_open()) {
            while (std::getline(osRelease, line)) {
                if (line.find("PRETTY_NAME") != std::string::npos) {
                    size_t pos = line.find('=') + 1;
                    if (pos < line.length()) {
                        version = line.substr(pos);
                        if (version.front() == '"' && version.back() == '"') {
                            version = version.substr(1, version.length() - 2);
                        }
                    }
                    break;
                }
            }
            osRelease.close();
        } else {
            version = "Failed to read OS information";
        }
        return version;
    }
#elif defined(__APPLE__)
    std::string GetSysVersion() {
        std::string version = "Unknown macOS Version";
        char versionX[256];
        size_t size = sizeof(versionX);
        if (sysctlbyname("kern.osproductversion", versionX, &size, nullptr, 0) == 0) {
            version = "macOS " + std::string(versionX);
        } else {
            version = "Failed to get macOS version";
        }
        return version;
    }
#else
    std::string GetSysVersion() {
        return "Unsupported Operating System";
    }
#endif

//    int GetSysBit() {
//#ifdef _WIN32
//        int bit = 0;
//        BOOL isWow64 = FALSE;
//        if (IsWow64Process(GetCurrentProcess(), &isWow64)) {
//            if (isWow64) {
//                bit = 64;
//            } else {
//                bit = 32;
//            }
//        } else {
//            bit = 0;
//        }
//        return bit;
//#elif defined(__linux__)
//        int bit = 0;
//        struct utsname sysInfo;
//        if (uname(&sysInfo) == 0) {
//            std::string machine = sysInfo.machine;
//            if (machine == "x86_64" || machine == "aarch64") {
//                bit = 64;
//            } else if (machine == "i386" || machine == "i686") {
//                bit = 32;
//            }
//        }
//        return bit;
//#elif defined(__APPLE__)
//        int bit = 0;
//        int is64Bit = 0;
//        size_t size = sizeof(is64Bit);
//        if (sysctlbyname("hw.cpu64bit_capable", &is64Bit, &size, nullptr, 0) == 0) {
//            bit = is64Bit ? 64 : 32;
//        }
//        return bit;
//#else
//        return 0;
//#endif
}