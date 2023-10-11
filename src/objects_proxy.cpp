#include "objects_proxy.hpp"

ObjectType GitObjectsProxy::getObjectType(SHAString hash) const {
    auto path = getFilePath(hash);
    if (path.has_value()) {
        auto ss = readFileAsStream(path.value());
        std::string type_s;
        int n;
        if (ss >> type_s >> n) {
            if (auto type = objectTypeFromString(type_s);
                type.has_value()) {
                return type.value();
            }
        }
        throw std::logic_error("cannot recognize object type");
    }
    else {
        throw std::invalid_argument(std::string("cannot find object ") + hash.data());
    }
}

ObjectType GitObjectsProxy::getObjectTypeNoExcept(SHAString hash) const noexcept {
    auto path = getFilePath(hash);
    if (path.has_value()) {
        auto ss = readFileAsStream(path.value());
        std::string type_s;
        int n;
        if (ss >> type_s >> n) {
            if (auto type = objectTypeFromString(type_s);
                type.has_value()) {
                return type.value();
            }
        }
    }
    return ObjectType::kUnknownObject;
}

//Option<std::stringstream> GitObjectsProxy::readObjectNoHeader(SHAString hash) const {
//    auto content = readObject(hash);
//    if (content.has_value()) {
//        auto &ss = content.value();
//        std::string s;
//        int n;
//        if (ss >> s >> n) {
//            if (objectTypeFromString(s).has_value() && n >= 0) {
//                ss.ignore();
//                s.resize(n + 1);
//                ss.read(s.data(), n);
//                if (strlen(s.c_str()) == n) {
//                    return std::stringstream(s);
//                }
//            }
//        }
//    }
//    return {};
//}

Option<std::stringstream> GitObjectsProxy::readObjectNoHeader(SHAString hash) const {
    using std::stringstream;
    FP::IO<std::stringstream> ss(GitObjectsProxy::getInstance().readObject(hash));

    int n;
    auto readHeader = [&n](stringstream &ss)->Option<std::string> {
        std::string s;
        if (ss >> s >> n) {
            ss.ignore();
            return s;
        }
        else
            return {};
    };
    auto object_type = ss.flatMap<std::string>(readHeader)
                        .flatMap<ObjectType>(objectTypeFromString);

    if (object_type.nonEmpty() && n >= 0) {
        auto readRemainder = [&n](stringstream &ss){
          std::string s;
          s.resize(n + 1);
          ss.read(s.data(), n);
          return s;
        };
        return ss.map<std::string>(readRemainder)
            .filter([&n](const std::string &s){return strlen(s.c_str()) == n;})
            .map<stringstream>([](const std::string &s){return stringstream(s);});
    }
    else {
        return {};
    }
}