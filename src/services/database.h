//
// Created by reverier on 2021/10/21.
//

#ifndef MEOWCLIENT_DATABASE_H
#define MEOWCLIENT_DATABASE_H

#include <QObject>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

class Database : public QObject {
Q_OBJECT
private:
    QSqlDatabase db_;
    QString query_sql_ = ("SELECT id, value FROM users WHERE value > %1 AND value < %2;");

public:
    Database(const QString &address, int port, const QString &user, const QString &password, const QString &database);

    /// default to all values.
    QStringList getValues(int min_val = -1, int max_val = -1);
};


#endif //MEOWCLIENT_DATABASE_H
