#pragma once

#include <functional>
#include <optional>

namespace FP {
struct Unit {};

template<typename Func>
using FunctionArg = const std::function<Func> &;

template<typename Inner>
struct Option : public std::optional<Inner> {
    using Super = std::optional<Inner>;
    using Super::Super;

    Option(std::optional<Inner> &&rhs) : Super(std::move(rhs)) {}

    const Inner &get() { return this->value(); }
    inline bool isEmpty() {return !this->has_value();}
    inline bool nonEmpty() {return this->has_value();}

    // monad operations
    /**
     * @brief Perform a function that do not fail. Func arg is expected to pass by const&
     */
    template<typename NewType>
    Option<NewType> map(FunctionArg<NewType(const Inner &)> fn) {
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
    Option<NewType> flatMap(FunctionArg<Option<NewType>(const Inner &)> fn) {
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
    Option<Inner> orElse(FunctionArg<Inner()> fn) {
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
    Option<Inner> orElse(FunctionArg<void()> fn) {
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
    Option<NewType> flatMap(FunctionArg<Option<NewType>(const Inner &)> fn, FunctionArg<void()> log_fn) {
        bool may_log = this->has_value();
        auto res = flatMap(fn);
        if (may_log)
            return res.orElse(log_fn);
        else
            return {};
    }

    Option<Inner> filter(FunctionArg<bool(const Inner &)> predicate) {
        return nonEmpty() && predicate(this->value()) ? *this : std::nullopt;
    }
};

template<typename Inner>
struct IO : public Option<Inner> {
    using Super = Option<Inner>;
    using Super::Super;

    /**
     * @brief Perform a function that do not fail. This func is meant for IO monad
     */
    template<typename NewType>
    IO<NewType> map(FunctionArg<NewType(Inner &)> fn) {
        if (this->has_value()) {
            return fn(this->value());
        } else {
            return {};
        }
    }

    template<typename NewType>
    IO<NewType> flatMap(FunctionArg<Option<NewType>(Inner &)> fn) {
        if (this->has_value()) {
            return fn(this->value());
        } else {
            return {};
        }
    }

    IO<Inner> filter(FunctionArg<bool(const Inner &)> predicate) {
        return this->nonEmpty() && predicate(this->value()) ? *this : std::nullopt;
    }
};
}

using FP::Unit;
using FP::Option;