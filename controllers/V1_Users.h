#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace V1 {
class Users : public drogon::HttpController<Users> {
   public:
    METHOD_LIST_BEGIN
    METHOD_ADD(Users::createUser, "", Post);
    METHOD_ADD(Users::getUser, "/{1}", Get);
    METHOD_ADD(Users::updateUser, "/{1}", Put);
    METHOD_ADD(Users::deleteUser, "/{1}", Delete);
    METHOD_ADD(Users::getAllUsers, "", Get);

    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string
    // p2); void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback,
    // double p1, int p2) const;

    void createUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
    void getUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback,
                 const std::string& userId);
    void updateUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback,
                    const std::string& userId);
    void deleteUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback,
                    const std::string& userId);
    void getAllUsers(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
};
}  // namespace V1
