/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */
#include "mainwindow.h"
#include <QApplication>
#include "framelessExtended/framelesshandle.h"
#include "customstyle.h"

#include <QtSingleApplication>
#include <QTranslator>
#include <fcntl.h>
#include <syslog.h>
#include <QObject>
#include <QDesktopWidget>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QTimer>
#include <QDebug>
#include <QGSettings>
#include <QSharedPointer>
#include <memory>
#include <X11/Xlib.h>

void centerToScreen(QWidget* widget) {
    if (!widget)
      return;
    QDesktopWidget* m = QApplication::desktop();
    QRect desk_rect = m->screenGeometry(m->screenNumber(QCursor::pos()));
    int desk_x = desk_rect.width();
    int desk_y = desk_rect.height();
    int x = widget->width();
    int y = widget->height();
    widget->move(desk_x / 2 - x / 2 + desk_rect.left(), desk_y / 2 - y / 2 + desk_rect.top());
}

int main(int argc, char *argv[])
{

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QtSingleApplication a(argc, argv);

    if (a.isRunning()) {
        a.sendMessage(QApplication::arguments().length() > 1 ? QApplication::arguments().at(1) : a.applicationFilePath());
        qDebug() << QObject::tr("ukui-control-center is already running!");
        return EXIT_SUCCESS;
    } else {

        // 加载国际化文件
        QTranslator translator;
        translator.load("/usr/share/ukui-control-center/shell/res/i18n/" + QLocale::system().name());
        a.installTranslator(&translator);

        //命令行参数
        QCoreApplication::setApplicationName("ukui-control-center");
        QCoreApplication::setApplicationVersion("2.0");
        QCommandLineParser parser;
        QCommandLineOption monitorRoleOption("m", "Go to monitor settings page");
        QCommandLineOption backgroundRoleOption("b", "Go to background settings page");
        QCommandLineOption userinfoRoleOption("u", "Go to userinfo settings page");
        QCommandLineOption aboutRoleOption("a", "Go to about settings page");
        QCommandLineOption powerRoleOption("p", "Go to power settings page");
        QCommandLineOption datetimeRoleOption("t", "Go to datetime settings page");
        QCommandLineOption desktopRoleOption("d", "Go to desktop settings page");
        QCommandLineOption audioRoleOption("s", "Go to audio settings page");
        QCommandLineOption noticeRoleOption("n", "Go to notice settings page");
        QCommandLineOption vpnRoleOption("v", "Go to vpn settings page");

        parser.addHelpOption();
        parser.addVersionOption();
        parser.addOption(monitorRoleOption);
        parser.addOption(backgroundRoleOption);
        parser.addOption(userinfoRoleOption);
        parser.addOption(aboutRoleOption);
        parser.addOption(powerRoleOption);
        parser.addOption(datetimeRoleOption);
        parser.addOption(desktopRoleOption);
        parser.addOption(audioRoleOption);
        parser.addOption(noticeRoleOption);
        parser.addOption(vpnRoleOption);
        parser.process(a);

        MainWindow * w = new MainWindow;
        centerToScreen(w);
        w->setAttribute(Qt::WA_DeleteOnClose);

        a.setActivationWindow(w);
        QObject::connect(&a, SIGNAL(messageReceived(const QString&)),w, SLOT(sltMessageReceived(const QString&)));
        w->show();

        FramelessHandle * pHandle = new FramelessHandle(w);
        pHandle->activateOn(w);

//        auto style = new InternalStyle(nullptr);
//        a.setStyle(style);

        return a.exec();
    }
}

