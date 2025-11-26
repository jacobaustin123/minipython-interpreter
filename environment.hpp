#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <unordered_map>
#include <string>
#include <memory>
#include <stdexcept>
#include "ast.hpp"

class RuntimeError : public std::runtime_error {
public:
    int line;
    RuntimeError(const std::string& msg, int line = 0)
        : std::runtime_error(msg), line(line) {}
};

class AssertionError : public std::runtime_error {
public:
    int line;
    AssertionError(const std::string& msg, int line = 0)
        : std::runtime_error(msg), line(line) {}
};

class Environment {
public:
    std::shared_ptr<Environment> enclosing;

    Environment() : enclosing(nullptr) {}
    explicit Environment(std::shared_ptr<Environment> enclosing)
        : enclosing(std::move(enclosing)) {}

    void define(const std::string& name, PyValue value) {
        values[name] = std::move(value);
    }

    PyValue get(const std::string& name) const {
        auto it = values.find(name);
        if (it != values.end()) {
            return it->second;
        }

        if (enclosing) {
            return enclosing->get(name);
        }

        throw RuntimeError("Undefined variable '" + name + "'");
    }

    void assign(const std::string& name, PyValue value) {
        auto it = values.find(name);
        if (it != values.end()) {
            it->second = std::move(value);
            return;
        }

        if (enclosing) {
            enclosing->assign(name, std::move(value));
            return;
        }

        // In Python, assignment creates a new variable if it doesn't exist
        values[name] = std::move(value);
    }

    bool contains(const std::string& name) const {
        if (values.find(name) != values.end()) {
            return true;
        }
        if (enclosing) {
            return enclosing->contains(name);
        }
        return false;
    }

private:
    std::unordered_map<std::string, PyValue> values;
};

#endif // ENVIRONMENT_HPP
