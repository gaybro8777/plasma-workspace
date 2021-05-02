/*
 *   Copyright 2017 Marco MArtin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; either version 2,
 *   or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "kcm.h"

#include <iostream>

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>

// Frameworks
#include <KAboutData>
#include <KLocalizedString>

#include <KPackage/Package>
#include <KPackage/PackageLoader>

#include "lookandfeelsettings.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    const char version[] = "1.0";

    // About data
    KAboutData aboutData("plasma-apply-lookandfeel",
                         i18n("Global Theme Tool"),
                         version,
                         i18n("Command line tool to apply global theme packages for changing the look and feel."),
                         KAboutLicense::GPL,
                         i18n("Copyright 2017, Marco Martin"));
    aboutData.addAuthor(i18n("Marco Martin"), i18n("Maintainer"), QStringLiteral("mart@kde.org"));
    aboutData.setDesktopFileName("org.kde.plasma-apply-lookandfeel");
    KAboutData::setApplicationData(aboutData);

    const static auto _l = QStringLiteral("list");
    const static auto _a = QStringLiteral("apply");
    const static auto _r = QStringLiteral("resetLayout");

    QCommandLineOption _list = QCommandLineOption(QStringList() << QStringLiteral("l") << _l, i18n("List available global theme packages"));
    QCommandLineOption _apply =
        QCommandLineOption(QStringList() << QStringLiteral("a") << _a,
                           i18n("Apply a global theme package. This can be the name of a package, or a full path to an installed package, at which point this "
                                "tool will ensure it is a global theme package and then attempt to apply it"),
                           i18n("packagename"));
    QCommandLineOption _resetLayout = QCommandLineOption(QStringList() << _r, i18n("Reset the Plasma Desktop layout"));

    QCommandLineParser parser;
    parser.addOption(_list);
    parser.addOption(_apply);
    parser.addOption(_resetLayout);
    aboutData.setupCommandLine(&parser);

    parser.process(app);
    aboutData.processCommandLine(&parser);

    if (!parser.isSet(_list) && !parser.isSet(_apply)) {
        parser.showHelp();
    }

    if (parser.isSet(_list)) {
        const QList<KPluginMetaData> pkgs = KPackage::PackageLoader::self()->listPackages("Plasma/LookAndFeel");

        for (const KPluginMetaData &data : pkgs) {
            std::cout << data.pluginId().toStdString() << std::endl;
        }

    } else if (parser.isSet(_apply)) {
        QString requestedTheme{parser.value(_apply)};
        QFileInfo info(requestedTheme);
        // Check if the theme name passed validates as the absolute path for a folder
        if (info.isDir()) {
            // absolute paths need to be passed with trailing shash to KPackage
            requestedTheme += QStringLiteral("/");
        }
        KPackage::Package p = KPackage::PackageLoader::self()->loadPackage("Plasma/LookAndFeel");
        p.setPath(requestedTheme);

        // can't use package.isValid as lnf packages always fallback, even when not existing
        if (p.metadata().pluginId() != requestedTheme) {
            if (!p.path().isEmpty() && p.path() == requestedTheme && QFile(p.path()).exists()) {
                std::cout << "Absolute path to theme passed in, set that" << std::endl;
                requestedTheme = p.metadata().pluginId();
            } else {
                std::cout << "Unable to find the theme named " << requestedTheme.toStdString() << std::endl;
                return 1;
            }
        }

        KCMLookandFeel *kcm = new KCMLookandFeel(nullptr, QVariantList());
        kcm->load();
        kcm->setResetDefaultLayout(parser.isSet(_resetLayout));
        kcm->lookAndFeelSettings()->setLookAndFeelPackage(requestedTheme);
        // Save manually as we aren't in an event loop
        kcm->lookAndFeelSettings()->save();
        kcm->save();
        delete kcm;
    }

    return 0;
}
