//
// Created by reverier on 2021/10/21.
//

#include <QJsonArray>
#include "handler.h"
#include "utils/crypto_utils.h"


MainHandler::MainHandler() {
    this->router_.addRoute("POST", "^/data-request$", this, &MainHandler::handleDataRequest);
    this->router_.addRoute("POST", "^/set-result$", this, &MainHandler::handleSetResult);
}

HttpPromise MainHandler::handle(HttpDataPtr data) {
    bool foundRoute;
    HttpPromise promise = this->router_.route(data, &foundRoute);
    if (foundRoute)
        return promise;

    qWarning("Request URI in this method is not found.");

    QJsonObject object;
    object["message"] = "INVALID URI or Method.";

    data->response->setStatus(HttpStatus::Forbidden, QJsonDocument(object));
    return HttpPromise::resolve(data);
}

HttpPromise MainHandler::handleDataRequest(HttpDataPtr data) {
    if (data->request->mimeType().compare("application/json", Qt::CaseInsensitive) != 0) {
        qWarning("Incorrect data type in handleTransformToSamePk.");
        throw HttpException(HttpStatus::BadRequest, "Request body content type must be application/json");
    }
    QJsonDocument jsonDocument = data->request->parseJsonBody();
    if (jsonDocument.isNull()) {
        qWarning("Incorrect data format in handleTransformToSamePk.");
        throw HttpException(HttpStatus::BadRequest, "Invalid JSON body");
    }

    MeowCryptoUtils::PublicParameters param;
    auto obj = jsonDocument.object()["data"].toObject();
    try {
        param.setN(obj["N"].toString());
        param.setK(obj["k"].toString());
        param.setG(obj["g"].toString());
        this->param_ = param;
    } catch (std::exception &exception) {
        throw HttpException(HttpStatus::BadRequest, "Bad data");
    }

    this->keys_ = MeowCryptoUtils::keyGen(param);

    qInfo("New key generated: PublicKey=%s, SecretKey=%s", this->keys_.publicKey().toStdString().c_str(),
          this->keys_.secretKey().toStdString().c_str());

    int min_val = -1, max_val = -1;

    if (obj.contains("filters")) {
        auto filters = obj["filters"].toObject();
        min_val = filters["min"].toInt();
        max_val = filters["max"].toInt();
    }

    auto values = this->conn_->getValues(min_val, max_val);

    MeowCryptoUtils::EncryptedPair res;

    QJsonObject ans;
    ans["public_key"] = this->keys_.publicKey();
    QJsonArray enc_arr;

    for (auto &i: values) {
        res = MeowCryptoUtils::encrypt(param, this->keys_.publicKey(), i);
        QJsonArray enc;
        enc.append(res.A());
        enc.append(res.B());
        enc_arr.append(enc);
        qInfo("Encrypted: %s -> %s#%s", i.toStdString().c_str(), res.A().toStdString().c_str(),
              res.B().toStdString().c_str());
    }

    ans["encrypted"] = enc_arr;

    QJsonObject resp;
    resp["message"] = "ok";
    resp["data"] = ans;

    data->response->setStatus(HttpStatus::Ok, QJsonDocument(resp));

    return HttpPromise::resolve(data);
}

HttpPromise MainHandler::handleSetResult(HttpDataPtr data) {
    if (data->request->mimeType().compare("application/json", Qt::CaseInsensitive) != 0) {
        qWarning("Incorrect data type in handleTransformToSamePk.");
        throw HttpException(HttpStatus::BadRequest, "Request body content type must be application/json");
    }
    QJsonDocument jsonDocument = data->request->parseJsonBody();
    if (jsonDocument.isNull()) {
        qWarning("Incorrect data format in handleTransformToSamePk.");
        throw HttpException(HttpStatus::BadRequest, "Invalid JSON body");
    }

    auto obj = jsonDocument.object().value("data").toObject();

    MeowCryptoUtils::EncryptedPair rec;
    rec.setA(obj["encrypted_pair_a"].toString());
    rec.setB(obj["encrypted_pair_b"].toString());
    rec.setPublicN(this->param_.N());
    rec.setPublicKey(this->keys_.publicKey());

    auto ans = MeowCryptoUtils::decrypt(this->param_, this->keys_.secretKey(), rec);

    qInfo("**Answer Received!!** The final RESULT is: %d", ans.toInt(nullptr, 16));

    data->response->setStatus(HttpStatus::Ok, QJsonDocument::fromJson(R"({"message":"ok"})"));

    return HttpPromise::resolve(data);
}

void MainHandler::setDatabaseConnection(Database *conn) {
    this->conn_ = conn;
}


//HttpPromise MainHandler::handleTransformBack(HttpDataPtr data) {
//    if (data->request->mimeType().compare("application/json", Qt::CaseInsensitive) != 0) {
//        qWarning("Incorrect data type in handleTransformToSamePk.");
//        throw HttpException(HttpStatus::BadRequest, "Request body content type must be application/json");
//    }
//    QJsonDocument jsonDocument = data->request->parseJsonBody();
//    if (jsonDocument.isNull()) {
//        qWarning("Incorrect data format in handleTransformToSamePk.");
//        throw HttpException(HttpStatus::BadRequest, "Invalid JSON body");
//    }
//
//    MeowCryptoUtils::EncryptedPair inp;
//    inp.setPublicKey(this->prod_pk_);
//    inp.setPublicN(this->publicParameters_.N());
//    inp.setA(jsonDocument.object()["encrypted_pair_a"].toString());
//    inp.setB(jsonDocument.object()["encrypted_pair_b"].toString());
//
//    QJsonArray ans;
//
//    for (auto &pk : this->pk_reverse_map_.keys()) {
//        auto tag = this->pk_reverse_map_[pk];
//        auto r = MeowCryptoUtils::masterTransform(this->publicParameters_, this->sk_, pk, inp);
//
//        QJsonArray record;
//        record.append(tag); // tag
//        record.append(r.A()); // encrypted pair A
//        record.append(r.B()); // encrypted pair B
//        ans.append(record);
//    }
//
//    QJsonObject resp;
//    resp["data"] = ans;
//    resp["message"] = "ok";
//
//    data->response->setStatus(HttpStatus::Ok, QJsonDocument(resp));
//
//    return HttpPromise::resolve(data);
//}
