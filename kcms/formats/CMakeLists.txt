# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcmformats\")

set(kcm_formats_PART_SRCS
    kcmformats.cpp
    localelistmodel.cpp
    formatssettings.cpp
    exampleutility.cpp
    optionsmodel.cpp
)

kconfig_add_kcfg_files(kcm_formats_PART_SRCS formatssettings.kcfgc GENERATE_MOC)

add_library(kcm_formats MODULE ${kcm_formats_PART_SRCS})

target_link_libraries(kcm_formats
    Qt5::Core
    KF5::ConfigCore
    KF5::ConfigGui
    KF5::CoreAddons
    KF5::I18n
    KF5::QuickAddons)


########### install files ###############
install(TARGETS kcm_formats  DESTINATION ${KDE_INSTALL_PLUGINDIR}/kcms )
install(FILES kcm_formats.desktop DESTINATION ${KDE_INSTALL_APPDIR})
kpackage_install_package(package kcm_formats kcms)
