#pragma once

#include <functional>
#include <optional>

template <typename T>
struct Option: public std::optional<T> {
    using Super = std::optional<T>;
    using Super::Super;

    /**
     * @brief Perform a function that do not fail. Func arg is expected to pass by const&
     */
    template<typename NewType>
    Option<NewType> map(const std::function<NewType(const T&)> &fn) {
        if (this->has_value()) {
            return fn(this->value());
        }
        else {
            return {};
        }
    }

    template<typename NewType>
    using MayFailFuncType = std::function< Option<NewType>(T) >;

    using LogFuncType = std::function<void()>;

    /**
     * @brief Perform a function that may fail.
     */
    template<typename NewType>
    Option<NewType> flatMap(const MayFailFuncType<NewType> &fn) {
        if (this->has_value()) {
            return fn(this->value());
        }
        else {
            return {};
        }
    }

    /**
     * @brief Perform a function that may fail. When failed, do `log_fn`
     */
    template<typename NewType>
    Option<NewType> flatMap(const MayFailFuncType<NewType> &fn, const LogFuncType &log_fn) {
        if (this->has_value()) {
            auto res = fn(this->value());
            if (!res.has_value()) {
                log_fn();
            }
            return res;
        }
        else {
            return {};
        }
    }
};