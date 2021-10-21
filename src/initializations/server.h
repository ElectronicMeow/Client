//
// Created by reverier on 2021/10/21.
//

#ifndef MEOWCLIENT_SERVER_H
#define MEOWCLIENT_SERVER_H

#include "http/httpServer.h"
#include <QSettings>
#include <QFile>
#include <QFileInfo>
#include "services/handler.h"
#include "utils/crypto_utils.h"


HttpServer *initServer() {
    const QStringList CONFIG_FILE_PATH = {
            "~/.config/",
            "/etc/",
            "/opt/",
            "./",
    };
    QString config_file;
    bool ok = false;
    for (auto &i : CONFIG_FILE_PATH) {
        config_file = i + "meow_client.conf";
        if (QFile::exists(config_file)) {
            ok = true;
            break;
        }
    }

    if (!ok) {
        qWarning("Could not found server configurations!");
        qWarning("Server Address default to 127.0.0.1:7301.");
    }

    QSettings settings(config_file, QSettings::IniFormat);
    settings.beginGroup("Server");
    auto address = settings.value("Address", "127.0.0.1").toString();
    auto port = settings.value("Port", 7301).toInt();
    settings.endGroup();

    settings.beginGroup("Database");

    auto db_address = settings.value("Address", "127.0.0.1").toString();
    auto db_port = settings.value("Port", 3306).toInt();
    auto db_user = settings.value("User", "db").toString();
    auto db_password = settings.value("Password", "db-sample-password").toString();
    auto db_database = settings.value("Database", "db").toString();

    auto *db = new Database(db_address, db_port, db_user, db_password, db_database);

    settings.endGroup();

    auto server_config = HttpServerConfig();
    server_config.host = QHostAddress(address);
    server_config.port = port;
    server_config.verbosity = HttpServerConfig::Verbose::Info;

    auto handler = new MainHandler();

    handler->setDatabaseConnection(db);

    auto server = new HttpServer(server_config, handler);
    qInfo("Server is running on %s:%d !", address.toStdString().c_str(), port);

    return server;
}

#endif //MEOWCLIENT_SERVER_H
