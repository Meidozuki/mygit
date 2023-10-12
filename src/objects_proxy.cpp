#include "objects_proxy.hpp"

struct HeaderInfo{
    bool valid;
    ObjectType type;
    int length;
};

HeaderInfo readHeader(std::stringstream &ss){
    HeaderInfo info{false, ObjectType::kBlob, 0};
    std::string s;
    if (ss >> s >> info.length && info.length >= 0) {
        ss.ignore();
        std::optional<ObjectType> type = objectTypeFromString(s);
        if (type.has_value()) {
            info.valid = true;
            info.type = type.value();
        }
    }
    return info;
}

ObjectType GitObjectsProxy::getObjectType(SHAString hash) const {
    auto path = getFilePath(hash);
    if (path.has_value()) {
        auto ss = readFileAsStream(path.value());
        if (HeaderInfo info = readHeader(ss);
            info.valid) {
            return info.type;
        }
        else
            throw std::logic_error("cannot recognize object type");
    }
    else {
        throw std::invalid_argument(std::string("cannot find object ") + hash.data());
    }
}

ObjectType GitObjectsProxy::getObjectTypeNoThrow(SHAString hash) const noexcept {
    auto path = getFilePath(hash);
    if (path.has_value()) {
        auto ss = readFileAsStream(path.value());
        if (HeaderInfo info = readHeader(ss);
            info.valid) {
            return info.type;
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

    HeaderInfo header_info = readHeader(ss.value());
    
    if (header_info.valid) {
        int n=header_info.length;
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

Option<Commit> GitObjectsProxy::readCommitObject(SHAString hash) {
    FP::IO<std::stringstream> ss = getFilePath(hash)
        .map<std::stringstream>([](const Path &path){
        return readFileAsStream(path);
    });

    auto header = ss.map<HeaderInfo>(readHeader)
                .filter([](const HeaderInfo &header){return header.valid && header.type == ObjectType::kCommit;});

    if (header.nonEmpty()) {
        return ss.map<Commit>([](std::stringstream &ss){
            Commit re;
            std::string s1,s2;
            ss >> s1 >> s2;
            re.tree_ = SHA1Proxy::getInstance().get_safe(s2.c_str());
            ss >> s1 >> s2;
            re.parent_ = SHA1Proxy::getInstance().get_safe(s2.c_str());
            ss >> s1 >> re.author_ >> s2 >> re.committer_;
            ss.ignore();ss.ignore();

            std::getline(ss,re.message_, '\0');
            return re;
        });
    }
    return {};
}
