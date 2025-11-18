#include "V1_Users.h"

#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>

#include <map>
using namespace V1;

// Add definition of your processing function here

struct UserSchema {
    std::string name;
    std::string email;
    std::string userId;
    int age;
    std::string password;
    std::map<std::string, std::string> metadata;
};

void sendResponseForNotFound(std::string message, const HttpRequestPtr& req,
                             std::function<void(const HttpResponsePtr&)>& callback) {
    Json::Value response;
    response["status"] = "error";
    response["message"] = message;
    auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
    callback(resp);
}

Json::Value userToJson(const UserSchema& user) {
    Json::Value json;

    json["name"] = user.name;
    json["email"] = user.email;
    json["age"] = user.age;
    json["userId"] = user.userId;

    Json::Value metaObj(Json::objectValue);
    for (const auto& p : user.metadata) {
        metaObj[p.first] = p.second;
    }
    json["metadata"] = metaObj;

    return json;
}

// global variable
std::map<std::string, UserSchema> users;

void Users::createUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    auto jsonPtr = req->getJsonObject();

    if (!jsonPtr) {
        Json::Value response;
        response["status"] = "error";
        response["message"] = "invalid json";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
        callback(resp);
        return;
    }
    UserSchema user;
    user.name = (*jsonPtr).get("name", "").asString();

    if (user.name.empty()) {
        sendResponseForNotFound("name is required", req, callback);
        return;
    }

    user.email = (*jsonPtr).get("email", "").asString();

    if (user.email.empty()) {
        sendResponseForNotFound("email is required", req, callback);
        return;
    }

    user.age = (*jsonPtr).get("age", 0).asInt();

    if (user.age <= 0) {
        sendResponseForNotFound("age is required", req, callback);
        return;
    }

    user.userId = (*jsonPtr).get("userId", "").asString();

    if (user.userId.empty()) {
        sendResponseForNotFound("userId is required", req, callback);
        return;
    }

    user.password = (*jsonPtr).get("password", "").asString();

    if (user.password.empty()) {
        sendResponseForNotFound("password is required", req, callback);
        return;
    }

    Json::Value metadataJson = (*jsonPtr)["metadata"];
    std::map<std::string, std::string> metadata;

    if (metadataJson.isObject()) {
        for (auto it = metadataJson.begin(); it != metadataJson.end(); ++it) {
            metadata[it.name()] = (*it).asString();
        }
    }

    user.metadata = metadata;

    users[user.userId] = user;
    Json::Value response;
    response["status"] = "success";
    response["message"] = "user created";
    auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
    callback(resp);
}

void Users::getUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback,
                    const std::string& userId) {
    if (userId.empty()) {
        sendResponseForNotFound("userId is required", req, callback);
        return;
    }
    Json::Value response;

    // Check if user exists
    if (users.find(userId) == users.end()) {
        response["status"] = "error";
        response["message"] = "User not found";

        auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
        callback(resp);
        return;
    }

    // Retrieve user
    const auto& user = users.at(userId);

    response["status"] = "success";
    response["message"] = "User found";
    response["user"] = userToJson(user);

    auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
    callback(resp);
}

void Users::getAllUsers(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    Json::Value response;
    response["status"] = "success";
    response["message"] = "Users found";
    Json::Value usersJson(Json::arrayValue);
    for (const auto& user : users) {
        usersJson.append(userToJson(user.second));
    }
    response["users"] = usersJson;
    auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
    callback(resp);
}

void Users::deleteUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback,
                       const std::string& userId) {
    if (userId.empty()) {
        sendResponseForNotFound("userId is required", req, callback);
        return;
    }

    if (users.find(userId) == users.end()) {
        Json::Value response;
        response["status"] = "error";
        response["message"] = "User not found";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
        callback(resp);
        return;
    }
    Json::Value response;
    users.erase(userId);
    response["status"] = "success";
    response["message"] = "User deleted";
    auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
    callback(resp);
}

void Users::updateUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback,
                       const std::string& userId) {
    if (userId.empty()) {
        sendResponseForNotFound("userId is required", req, callback);
        return;
    }

    std::string token = req->getParameter("token");

    if (token.empty()) {
        sendResponseForNotFound("token is required", req, callback);
        return;
    }

    std::cout << "token: " << token << std::endl;

    if (users.find(userId) == users.end()) {
        Json::Value response;
        response["status"] = "error";
        response["message"] = "User not found";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
        callback(resp);
        return;
    }

    auto jsonPtr = req->getJsonObject();

    if (!jsonPtr) {
        sendResponseForNotFound("Invalid json", req, callback);
        return;
    }

    UserSchema user = users.at(userId);

    std::string name = (*jsonPtr).get("name", "").asString();

    if (!name.empty()) {
        user.name = name;
    }

    int age = (*jsonPtr).get("age", 0).asInt();
    if (age > 0) {
        user.age = age;
    }

    Json::Value metadataJson = (*jsonPtr)["metadata"];

    if (metadataJson.isObject()) {
        for (auto it = metadataJson.begin(); it != metadataJson.end(); ++it) {
            user.metadata[it.name()] = it->asString();
        }
    }

    users[userId] = user;

    Json::Value response;
    response["status"] = "success";
    response["message"] = "User updated";
    auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
    callback(resp);
}