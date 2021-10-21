//
// Created by reverier on 2021/10/21.
//

#include "database.h"
#include <QVariant>

Database::Database(const QString &address, int port, const QString &user, const QString &password,
                   const QString &database) {
    if (QSqlDatabase::contains("qt_sql_default_connection"))
        this->db_ = QSqlDatabase::database("qt_sql_default_connection");
    else
        this->db_ = QSqlDatabase::addDatabase("QMYSQL");

    this->db_.setHostName(address);
    this->db_.setUserName(user);
    this->db_.setPort(port);
    this->db_.setPassword(password);
    this->db_.setDatabaseName(database);
    if (!this->db_.open()) {
        qCritical("Could not open database connections on (%s@%s:%d)/%s : %s", user.toStdString().c_str(),
                  address.toStdString().c_str(), port, database.toStdString().c_str(),
                  this->db_.lastError().text().toStdString().c_str());
    }
}

QStringList Database::getValues(int min_val, int max_val) {
    QSqlQuery query;
    query.exec(this->query_sql_.arg(min_val).arg(max_val));
    QStringList res{};
    while (query.next()) {
        res.append(QString::number(query.value(1).toInt()));
    }
    return res;
}
