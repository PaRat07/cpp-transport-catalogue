#include "json.h"
#include <vector>
#include <map>
#include <variant>
namespace json {
    class KeyItemContext;
    class ValueAfterKeyItemContext;
    class DictItemContext;
    class ArrayItemContext;
    class ValueInArrayItemContext;

    class Builder {
    public:
        KeyItemContext Key(std::string obj);

        template<class T>
        Builder& Value(T obj) {
            if (root_ == json::Node()) {
                root_ = std::move(obj);
            }
            else if (nodes_stack_.empty()) {
                throw std::logic_error("Value failed");
            }
            else if (nodes_stack_.back()->IsArray()) {
                std::get<std::vector<Node>>(nodes_stack_.back()->GetValue()).push_back(std::move(obj));
            }
            else if (nodes_stack_.back()->IsDict()) {
                if (!is_prev_key) {
                    throw std::logic_error("Value failed");
                }
                std::get<json::Dict>(nodes_stack_.back()->GetValue())[key_] = std::move(obj);
            }
            else {
                throw std::logic_error("Value failed");
            }
            is_prev_key = false;
            return *this;
        }
        DictItemContext StartDict();
        ArrayItemContext StartArray();
        Builder& EndDict();
        Builder& EndArray();
        json::Node Build() const;
    private:
        bool is_prev_key = false;
        std::string key_;
        json::Node root_;
        std::vector<Node*> nodes_stack_;
    };
    class KeyItemContext {
    public:
        KeyItemContext(Builder &builder);

        template<typename T>
        ValueAfterKeyItemContext Value(const T &obj);

        DictItemContext StartDict();

        ArrayItemContext StartArray();
    private:
        Builder &builder_;
    };

    class ValueAfterKeyItemContext {
    public:
        ValueAfterKeyItemContext(Builder &builder);

        KeyItemContext Key(const std::string &obj);
        Builder &EndDict();
    private:
        Builder &builder_;
    };

    class DictItemContext {
    public:
        DictItemContext(Builder &builder);

        KeyItemContext Key(const std::string &obj);
        Builder &EndDict();
    private:
        Builder &builder_;
    };

    class ArrayItemContext {
    public:
        ArrayItemContext(Builder &builder);

        template<typename T>
        ValueInArrayItemContext Value(const T &obj);

        DictItemContext StartDict();

        ArrayItemContext StartArray();

        Builder EndArray();
    private:
        Builder &builder_;
    };

    class ValueInArrayItemContext {
    public:
        ValueInArrayItemContext(Builder &builder);

        template<typename T>
        ValueInArrayItemContext Value(const T &obj);

        DictItemContext StartDict();

        ArrayItemContext StartArray();

        Builder &EndArray();
    private:
        Builder &builder_;
    };

    template<typename T>
    json::ValueAfterKeyItemContext json::KeyItemContext::Value(const T &obj) {
        builder_.Value(obj);
        return { builder_ };
    }

    template<typename T>
    json::ValueInArrayItemContext json::ArrayItemContext::Value(const T &obj) {
        builder_.Value(obj);
        return { builder_ };
    }

    template<typename T>
    json::ValueInArrayItemContext json::ValueInArrayItemContext::Value(const T &obj) {
        builder_.Value(obj);
        return { builder_ };
    }
}