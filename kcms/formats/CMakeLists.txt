# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcmformats\")

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
ecm_install_configured_files(INPUT org.kde.localegenhelper.service.in DESTINATION ${KDE_INSTALL_DBUSSERVICEDIR} @ONLY)
install(TARGETS plasma-localegen-helper ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES org.kde.localegenhelper.conf DESTINATION "/usr/share/dbus-1/system.d")
install(FILES org.kde.localegenhelper.policy DESTINATION "/usr/share/polkit-1/actions")
set(kcm_formats_PART_SRCS
    kcmformats.cpp
    localelistmodel.cpp
    formatssettings.cpp
    exampleutility.cpp
    optionsmodel.cpp
    languagelistmodel.cpp
    localegenerator.cpp
)
kconfig_add_kcfg_files(kcm_formats_PART_SRCS formatssettings.kcfgc GENERATE_MOC)

set(LocaleGenHelperXML ${CMAKE_CURRENT_BINARY_DIR}/org.kde.localegenhelper.xml)
qt5_add_dbus_interface(kcm_formats_PART_SRCS ${LocaleGenHelperXML} localegenhelperinterface )

ecm_qt_declare_logging_category(
    kcm_formats_PART_SRCS
    HEADER debug.h
    IDENTIFIER KCM_FORMATS
    CATEGORY_NAME org.kde.kcm_formats
    DESCRIPTION "Formats KCM"
    EXPORT kcm_formats
)

ecm_qt_install_logging_categories(
    EXPORT kcm_formats
    DESTINATION "${KDE_INSTALL_LOGGINGCATEGORIESDIR}"
)
kcoreaddons_add_plugin(kcm_formats SOURCES ${kcm_formats_PART_SRCS} INSTALL_NAMESPACE "kcms")
kcoreaddons_desktop_to_json(kcm_formats "kcm_formats.desktop")
target_link_libraries(kcm_formats
    KF5::I18n
    KF5::KCMUtils
    KF5::QuickAddons)
if(UBUNTU_PACKAGEKIT)
    target_link_libraries(kcm_formats PK::packagekitqt5)
endif()

########### install files ###############
install(TARGETS kcm_formats  DESTINATION ${KDE_INSTALL_PLUGINDIR}/kcms )
install(FILES kcm_formats.desktop DESTINATION ${KDE_INSTALL_APPDIR})
kpackage_install_package(package kcm_formats kcms)
