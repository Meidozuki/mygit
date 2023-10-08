#pragma once

#include <functional>
#include <optional>

template <typename T>
struct Option: public std::optional<T> {
    using Super = std::optional<T>;
    using Super::Super;

    const T& get() {return this->value();}

    // monad operations
    /**
     * @brief Perform a function that do not fail. Func arg is expected to pass by const&
     */
    template<typename NewType>
    Option<NewType> map(const std::function<NewType(const T&)> &fn) {
        if (this->has_value()) {
            return Option<NewType>(fn(this->value()));
        }
        else {
            return {};
        }
    }

    /**
     * @brief Perform a function that may fail.
     */
    template<typename NewType>
    Option<NewType> flatMap(const std::function< Option<NewType>(const T&)> &fn) {
        if (this->has_value()) {
            return fn(this->value());
        }
        else {
            return {};
        }
    }

    /**
     * @brief If the option is empty, invoke fn and return the result
     */
    Option<T> orElse(const std::function<T()> &fn) {
        if (!this->has_value()) {
            return fn();
        }
        else {
            return *this;
        }
    }

    /**
     * @brief If the option is empty, invoke the log function, then terminate the calling chain.
     * WARNING: Do not continue with this chain as when the next orElse() called, something unexpected will happen
     * @return Option[None]
     */
    Option<T> orElse(const std::function<void()> &fn) {
        if (!this->has_value()) {
            fn();
            return {};
        }
        else {
            return *this;
        }
    }

    /**
     * @brief A convenient wrapper. Perform a function that may fail. When failed, do `log_fn`
     */
    template<typename NewType>
    Option<NewType> flatMap(const std::function< Option<NewType>(const T&)> &fn, const std::function<void()> &log_fn) {
        bool may_log = this->has_value();
        auto res = flatMap(fn);
        if (may_log)
            return res.orElse(log_fn);
        else 
            return {};
    }
};