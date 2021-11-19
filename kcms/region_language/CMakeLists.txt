# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcmregionandlang\")

if(UBUNTU_PACKAGEKIT OR GLIBC_LOCALE_GEN)
    find_package(PolkitQt5-1)
    set(localegen_Helper_SRCS
        localegenhelper.cpp
    )
    qt5_generate_dbus_interface(
        ${CMAKE_CURRENT_SOURCE_DIR}/localegenhelper.h
        org.kde.localegenhelper.xml
        OPTIONS -s -m
    )
    qt5_add_dbus_adaptor(localegen_Helper_SRCS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.localegenhelper.xml
                        ${CMAKE_CURRENT_SOURCE_DIR}/localegenhelper.h LocaleGenHelper)
    add_executable(plasma-localegen-helper ${localegen_Helper_SRCS})
    target_link_libraries(plasma-localegen-helper
        Qt::DBus
        PolkitQt5-1::Core
    )
    ecm_install_configured_files(INPUT org.kde.localegenhelper.service.in DESTINATION ${DBUS_SYSTEM_SERVICES_INSTALL_DIR} @ONLY)
    install(TARGETS plasma-localegen-helper ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
    install(FILES org.kde.localegenhelper.conf DESTINATION "${CMAKE_INSTALL_PREFIX}/share/dbus-1/system.d")
    install(FILES org.kde.localegenhelper.policy DESTINATION "${CMAKE_INSTALL_PREFIX}/share/polkit-1/actions")
endif()

set(kcm_regionandlang_PART_SRCS
    kcmregionandlang.cpp
    localelistmodel.cpp
    regionandlangsettings.cpp
    exampleutility.cpp
    optionsmodel.cpp
    languagelistmodel.cpp
    localegenerator.cpp
)
kconfig_add_kcfg_files(kcm_regionandlang_PART_SRCS regionandlangsettings.kcfgc GENERATE_MOC)

if(UBUNTU_PACKAGEKIT OR GLIBC_LOCALE_GEN)
    set(LocaleGenHelperXML ${CMAKE_CURRENT_BINARY_DIR}/org.kde.localegenhelper.xml)
    qt5_add_dbus_interface(kcm_regionandlang_PART_SRCS ${LocaleGenHelperXML} localegenhelperinterface )
endif()

ecm_qt_declare_logging_category(
    kcm_regionandlang_PART_SRCS
    HEADER debug.h
    IDENTIFIER KCM_REGIONANDLANG
    CATEGORY_NAME org.kde.kcm_regionandlang
    DESCRIPTION "Region and Language KCM"
    EXPORT kcm_regionandlang
)

ecm_qt_install_logging_categories(
    EXPORT kcm_regionandlang
    DESTINATION "${KDE_INSTALL_LOGGINGCATEGORIESDIR}"
)

kcoreaddons_add_plugin(kcm_regionandlang SOURCES ${kcm_regionandlang_PART_SRCS} INSTALL_NAMESPACE "kcms")
kcoreaddons_desktop_to_json(kcm_regionandlang "kcm_regionandlang.desktop")
target_link_libraries(kcm_regionandlang
    KF5::I18n
    KF5::KCMUtils
    KF5::QuickAddons)
if(UBUNTU_PACKAGEKIT)
    target_link_libraries(kcm_regionandlang PK::packagekitqt5)
endif()
if(UBUNTU_PACKGEKIT OR GLIBC_LOCALE_GEN)
    target_link_libraries(kcm_regionandlang Qt::DBus)
endif()

########### install files ###############
install(TARGETS kcm_regionandlang  DESTINATION ${KDE_INSTALL_PLUGINDIR}/kcms )
install(FILES kcm_regionandlang.desktop DESTINATION ${KDE_INSTALL_APPDIR})
kpackage_install_package(package kcm_regionandlang kcms)
