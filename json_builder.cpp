#include "json_builder.h"

json::DictItemContext json::ValueInArrayItemContext::StartDict() {
    builder_.StartDict();
    return { builder_ };
}

json::ArrayItemContext json::ValueInArrayItemContext::StartArray() {
    builder_.StartArray();
    return { builder_ };
}

json::Builder &json::ValueInArrayItemContext::EndArray() {
    builder_.EndArray();
    return builder_;
}

json::ValueInArrayItemContext::ValueInArrayItemContext(json::Builder &builder)
        : builder_(builder)
{}

json::DictItemContext json::KeyItemContext::StartDict() {
    builder_.StartDict();
    return { builder_ };
}

json::ArrayItemContext json::KeyItemContext::StartArray() {
    builder_.StartArray();
    return { builder_ };
}

json::KeyItemContext::KeyItemContext(json::Builder &builder)
        : builder_(builder)
{}

json::KeyItemContext json::ValueAfterKeyItemContext::Key(const std::string &obj) {
    builder_.Key(obj);
    return { builder_ };
}

json::Builder &json::ValueAfterKeyItemContext::EndDict() {
    return builder_.EndDict();
    return { builder_ };
}

json::ValueAfterKeyItemContext::ValueAfterKeyItemContext(json::Builder &builder)
        : builder_(builder)
{}

json::KeyItemContext json::DictItemContext::Key(const std::string &obj) {
    builder_.Key(obj);
    return { builder_ };
}

json::Builder &json::DictItemContext::EndDict() {
    return builder_.EndDict();
}

json::DictItemContext::DictItemContext(json::Builder &builder)
        : builder_(builder)
{}

json::DictItemContext json::ArrayItemContext::StartDict() {
    builder_.StartDict();
    return { builder_ };
}

json::ArrayItemContext json::ArrayItemContext::StartArray() {
    builder_.StartArray();
    return { builder_ };
}

json::Builder json::ArrayItemContext::EndArray() {
    builder_.EndArray();
    return { builder_ };
}

json::ArrayItemContext::ArrayItemContext(json::Builder &builder)
        : builder_(builder)
{}

json::KeyItemContext json::Builder::Key(std::string obj) {
    if (nodes_stack_.empty()) {
        throw std::logic_error("Key failed");
    }
    if (!nodes_stack_.back()->IsDict()) {
        throw std::logic_error("Key failed");
    }
    if (is_prev_key) {
        throw std::logic_error("Key failed");
    }
    key_ = move(obj);
    is_prev_key = true;
    return { *this };
}

json::DictItemContext json::Builder::StartDict() {
    if (nodes_stack_.empty()) {
        if (root_ == json::Node()) {
            root_ = json::Dict();
            nodes_stack_.push_back(&root_);
        }
        else {
            throw std::logic_error("Starting dict failed");
        }
    }
    else if (nodes_stack_.back()->IsArray()) {
        std::get<json::Array>(nodes_stack_.back()->GetValue()).emplace_back(json::Dict());
        nodes_stack_.push_back(&std::get<json::Array>(nodes_stack_.back()->GetValue()).back());
    }
    else if (nodes_stack_.back()->IsDict()) {
        if (!is_prev_key) {
            throw std::logic_error("Starting dict failed");
        }
        std::get<json::Dict>(nodes_stack_.back()->GetValue())[key_] = json::Dict();
        nodes_stack_.push_back(&std::get<json::Dict>(nodes_stack_.back()->GetValue())[key_]);
    }
    else {
        throw std::logic_error("Starting dict failed");
    }
    is_prev_key = false;
    return { *this };
}

json::ArrayItemContext json::Builder::StartArray() {
    if (nodes_stack_.empty()) {
        if (root_ == json::Node()) {
            root_ = json::Array();
            nodes_stack_.push_back(&root_);
        }
        else {
            throw std::logic_error("Starting array failed");
        }
    }
    else if (nodes_stack_.back()->IsArray()) {
        std::get<json::Array>(nodes_stack_.back()->GetValue()).push_back(json::Array());
        nodes_stack_.push_back(&std::get<json::Array>(nodes_stack_.back()->GetValue()).back());
    }
    else if (nodes_stack_.back()->IsDict()) {
        if (!is_prev_key) {
            throw std::logic_error("Starting array failed");
        }
        std::get<json::Dict>(nodes_stack_.back()->GetValue())[key_] = json::Array();
        nodes_stack_.push_back(&std::get<json::Dict>(nodes_stack_.back()->GetValue())[key_]);
    }
    else {
        throw std::logic_error("Starting array failed");
    }
    is_prev_key = false;
    return { *this };
}

json::Builder &json::Builder::EndDict() {
    if (nodes_stack_.empty()) {
        throw std::logic_error("Ending dict failed");
    }
    if (!nodes_stack_.back()->IsDict()) {
        throw std::logic_error("Ending dict failed");
    }
    nodes_stack_.pop_back();
    return *this;
}

json::Builder &json::Builder::EndArray() {
    if (nodes_stack_.empty()) {
        throw std::logic_error("Ending array failed");
    }
    if (!nodes_stack_.back()->IsArray()) {
        throw std::logic_error("Ending array failed");
    }
    nodes_stack_.pop_back();
    return *this;
}

json::Node json::Builder::Build() const {
    if (!nodes_stack_.empty() || root_ == json::Node()) {
        throw std::logic_error("Building failed");
    }
    if (is_prev_key) {
        throw std::logic_error("Building failed");
    }
    return root_;
}
