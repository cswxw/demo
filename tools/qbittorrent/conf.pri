# Adapt these paths
# Point these to the include folders
INCLUDEPATH += $$quote(D:/qBittorrent/boost_1_64_0)
INCLUDEPATH += $$quote(D:/qBittorrent/libtorrent/include)
INCLUDEPATH += $$quote(D:/qBittorrent/zlib/)
INCLUDEPATH += $$quote(D:/qBittorrent/openssl-install-static/include)
# Point these to the lib folders
LIBS += $$quote(-LD:/qBittorrent/boost_1_64_0/lib64-msvc-14.0)
LIBS += $$quote(-LD:/qBittorrent/libtorrent/lib)
LIBS += $$quote(-LD:/qBittorrent/zlib/lib)
LIBS += $$quote(-LD:/qBittorrent/openssl-install-static/lib)

# Adapt the lib names/versions accordingly
# If you want to use Boost auto-linking then disable
# BOOST_ALL_NO_LIB below and omit Boost libraries here
CONFIG(debug, debug|release) {
    LIBS += libtorrentdx64-vs2015.lib \
            libboost_system-vc140-mt-gd-1_64.lib
}
else {
    LIBS += libtorrentx64-vs2013.lib \
            libboost_system-vc140-mt-s-1_64.lib
}
LIBS += libcrypto.lib libssl.lib
LIBS += zlibstatic.lib
LIBS += Crypt32.lib
LIBS += iphlpapi.lib
# ...or if you use MinGW
#CONFIG(debug, debug|release) {
#    LIBS += libtorrent-rasterbar \
#            libboost_system-mt
#}
#else {
#    LIBS += libtorrent-rasterbar \
#            libboost_system-mt
#}
#LIBS += libcrypto libssl
#LIBS += libz

DEFINES += NTDDI_VERSION=0x05010000
DEFINES += _WIN32_WINNT=0x0501
DEFINES += _WIN32_IE=0x0501

# Disable to use Boost auto-linking
DEFINES += BOOST_ALL_NO_LIB
# Use one of the following options
DEFINES += BOOST_SYSTEM_STATIC_LINK
#DEFINES += BOOST_SYSTEM_DYN_LINK
# Boost 1.60+ defaults to Vista+ support. The define below enables XP support again.
DEFINES += BOOST_USE_WINAPI_VERSION=0x0501
# Enable if building against libtorrent 1.0.x (RC_1_0) (static linking)
#DEFINES += BOOST_ASIO_SEPARATE_COMPILATION
# Enable if building against libtorrent 1.0.x (RC_1_0) (dynamic linking)
#DEFINES += BOOST_ASIO_DYN_LINK
# Enable if encountered build error with boost version <= 1.59
#DEFINES += BOOST_NO_CXX11_RVALUE_REFERENCES

# Enable if building against libtorrent 1.1.x (RC_1_1)
# built with this flag defined
#DEFINES += TORRENT_NO_DEPRECATE
# Enable if linking dynamically against libtorrent
#DEFINES += TORRENT_LINKING_SHARED

# Enable stack trace support
CONFIG += stacktrace

win32-msvc* {
    QMAKE_CXXFLAGS += "/guard:cf"
    QMAKE_LFLAGS += "/guard:cf"
    QMAKE_LFLAGS_RELEASE += "/OPT:REF /OPT:ICF"
}
