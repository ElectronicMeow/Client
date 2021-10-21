//
// Created by reverier on 2021/10/21.
//

#ifndef MEOWCLIENT_HANDLER_H
#define MEOWCLIENT_HANDLER_H

#include "http/httpRequestRouter.h"
#include "http/httpRequestHandler.h"
#include "utils/crypto_utils.h"
#include "database.h"

class MainHandler : public HttpRequestHandler {
private:
    HttpRequestRouter router_;
    Database * conn_;
    MeowCryptoUtils::KeyPair keys_;
    MeowCryptoUtils::PublicParameters param_;
    int result;

public:
    MainHandler();

    void setDatabaseConnection(Database* conn);

    HttpPromise handle(HttpDataPtr data) override;

    HttpPromise handleDataRequest(HttpDataPtr data);

    HttpPromise handleSetResult(HttpDataPtr data);
};


#endif //MEOWCLIENT_HANDLER_H
