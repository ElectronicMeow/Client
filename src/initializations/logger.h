//
// Created by reverier on 2021/10/20.
//

#ifndef MEOWCLIENT_LOGGER_H
#define MEOWCLIENT_LOGGER_H

#include <QMessageLogger>


void initLogger() {
    qSetMessagePattern("%{time [yyyy-MM-dd hh:mm:ss]} - "
                       "%{if-debug}\033[97;43m DBUG%{endif}"
                       "%{if-info}\033[97;44m INFO%{endif}"
                       "%{if-warning}\033[97;43m WARN%{endif}"
                       "%{if-critical}\033[97;45m EROR%{endif}"
                       "%{if-fatal}\033[97;41m FATAL%{endif} \033[0m %{message}");
}

#endif //MEOWCLIENT_LOGGER_H
