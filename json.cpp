#include "json.h"

using namespace std;

namespace json {

    namespace {

        Node LoadNode(istream& input);

        Node LoadArray(istream& input) {
            Array result;

            for (char c; input.peek() != -1 && input.peek() != ']' && input >> c;) {
                if (c != ',') {
                    input.putback(c);
                }
                while (input.peek() != -1 && input.peek() != ']' && (c == '\n' || c == '\r' || c == '\t' || c == ' ')) {
                    c = input.get();
                }
                if (input.peek() == ']') {
                    break;
                }
                result.push_back(LoadNode(input));
            }

            return Node(move(result));
        }

        Node LoadInt(std::istream& input) {
            string num;
            while (input.peek() == ' ') { input.get(); }
            while (input.peek() != -1 && input.peek() != ',' && input.peek() != '}' && input.peek() != ']' && input.peek() != ' ') {
                num.push_back(input.get());
            }
            if (count_if(num.begin(), num.end(), [](char sym) {
                return sym == '.' || sym == 'e' || sym == 'E';
            })) {
                try {
                    return stod(num);
                }
                catch (...) {
                    throw ParsingError("failed to convert " + num + " to double");
                }
            }
            else {
                try {
                    return stoi(num);
                }
                catch (...) {
                    throw ParsingError("failed to convert " + num + " to int");
                }
            }
        }

        Node LoadString(std::istream& input) {
            using namespace std::literals;

            string ans;
            bool is_last_backslash = false;
            for (char sym; input.peek() != -1 && (input.peek() != '"' || is_last_backslash) && (sym = input.get());) {
                if (sym == -1) {
                    break;
                }
                if (is_last_backslash) {
                    switch (sym) {
                        case 'n':
                            ans.push_back('\n');
                            break;
                        case 't':
                            ans.push_back('\t');
                            break;
                        case 'r':
                            ans.push_back('\r');
                            break;
                        case '"':
                            ans.push_back('"');
                            break;
                        case '\\':
                            ans.push_back('\\');
                            break;
                        default:
                            // Встретили неизвестную escape-последовательность
                            throw ParsingError("Unrecognized escape sequence \\"s + sym);
                    }
                    is_last_backslash = false;
                }
                else {
                    if (sym == '\\') {
                        is_last_backslash = true;
                    }
                    else {
                        ans.push_back(sym);
                    }
                }
            }
            return Node(ans);
        }

        Node LoadNull(istream& input) {
            std::string line;
            while (isalpha(input.peek())) { line.push_back(input.get()); }
            if (line != "null") {
                throw ParsingError(line + "isn't null");
            }
            return Node(nullptr);
        }

        Node LoadBool(istream& input) {
            string word;
            for (char sym; input.peek() != -1 && input.peek() != ',' && input.peek() != '}' && input.peek() != ']' && input.peek() != ' ' && (sym = input.get());) {
                word.push_back(sym);
            }
            if (word == "true") {
                return Node(true);
            }
            else if (word == "false") {
                return Node(false);
            }
            else {
                throw ParsingError("incorrect query: " + word);
            }
        }

        Node LoadDict(istream& input) {
            Dict result;
            for (char c; input.peek() != '}' && (c = input.get());) {
                if (c == ',') {
                    input >> c;
                }
                if (c != '"') {
                    while (input.peek() != -1 && input.peek() != '}' && c != '"') {
                        c = input.get();
                    }
                }
                if (input.peek() == -1 || input.peek() == '}') {
                    return Node(move(result));
                }
                string key = LoadString(input).AsString();
                if (input.peek() == -1 || input.peek() == '}') {
                    return Node(move(result));
                }
                input >> c;
                input >> c;
                result.insert({std::move(key), LoadNode(input)});
            }

            return Node(move(result));
        }

        Node LoadNode(istream& input) {
            char c;
            input >> c;

            if (c == '[') {
                auto ans = LoadArray(input);
                if (input.get() != ']') {
                    throw ParsingError("arr doesn't end with ]");
                }
                return ans;
            } else if (c == '{') {
                auto ans = LoadDict(input);
                if (input.get() != '}') {
                    throw ParsingError("dict doesn't end with ]");
                }
                return ans;
            } else if (c == '"') {
                auto ans = LoadString(input);
                if (input.get() != '"') {
                    throw ParsingError("string doesn't end with \"");
                }
                return ans;
            } else if (c == 't' or c == 'f') {
                input.putback(c);
                return LoadBool(input);
            }else if (c == 'n') {
                input.putback(c);
                try {
                    return LoadNull(input);
                }
                catch (const ParsingError &error) {
                    throw error;
                }
            } else {
                input.putback(c);
                try {
                    return LoadInt(input);
                }
                catch(...) {
                    throw ParsingError("failed parsing int or double");
                }
            }
        }

    }  // namespace

    Node::Node(Array array)
            : data_(move(array)) {
    }

    Node::Node(Dict map)
            : data_(move(map)) {
    }

    Node::Node(int value)
            : data_(value) {
    }

    Node::Node(double value)
            : data_(value) {
    }

    Node::Node(string value)
            : data_(value)
    {
    }

    Node::Node(std::nullptr_t)
            : data_(nullptr)
    {
    }
    Node::Node(bool value)
            : data_(value)
    {

    }

    const Array& Node::AsArray() const {
        if (!IsArray()) {
            throw logic_error("this node isn't array");
        }
        return get<Array>(data_);
    }

    const Dict& Node::AsMap() const {
        if (!IsMap()) {
            throw logic_error("this node isn't dict");
        }
        return get<Dict>(data_);
    }

    int Node::AsInt() const {
        if (!IsInt()) {
            throw logic_error("this node isn't int");
        }
        return get<int>(data_);
    }
    double Node::AsDouble() const {
        if (!IsDouble()) {
            throw logic_error("this node isn't double");
        }
        return IsPureDouble() ? get<double>(data_) : get<int>(data_);
    }

    bool Node::AsBool() const {
        if (!IsBool()) {
            throw logic_error("this node isn't int");
        }
        return get<bool>(data_);
    }

    const string& Node::AsString() const {
        if (!IsString()) {
            throw logic_error("this node isn't string");
        }
        return get<string>(data_);
    }

    bool Node::IsInt() const {
        return holds_alternative<int>(data_);
    }
    bool Node::IsDouble() const {
        return holds_alternative<int>(data_) || holds_alternative<double>(data_);
    }

    bool Node::IsPureDouble() const {
        return holds_alternative<double>(data_);
    }

    bool Node::IsBool() const {
        return holds_alternative<bool>(data_);
    }

    bool Node::IsString() const {
        return holds_alternative<string>(data_);
    }

    bool Node::IsNull() const {
        return holds_alternative<nullptr_t>(data_);
    }

    bool Node::IsArray() const {
        return holds_alternative<Array>(data_);
    }

    bool Node::IsMap() const {
        return holds_alternative<Dict>(data_);
    }

    Document::Document(Node root)
            : root_(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    Document Load(istream& input) {
        try {
            return Document{LoadNode(input)};
        }
        catch(...) {
            throw ParsingError("parsing failed");
        }
    }

    void Print(const Document& doc, std::ostream& output) {
        doc.GetRoot().PrintNode({output});
    }

    bool Node::operator==(const Node &other) const {
        return data_ == other.data_;
    }

    bool Node::operator!=(const Node &other) const {
        return !(*this == other);
    }
    std::variant<std::nullptr_t, int, double, std::string, bool, Array, Dict> Node::GetValue() const {
        return data_;
    }


    void Node::PrintValue(const int &value, const PrintContext& ctx) const {
        ctx.out << value;
    }

    void Node::PrintValue(const bool &value, const PrintContext& ctx) const {
        ctx.out << (value ? "true" : "false");
    }

    void Node::PrintValue(const std::string &value, const PrintContext& ctx) const {
        ctx.out << '"';
        for (char sym : value) {
            switch (sym) {
                case '\n':
                    ctx.out << "\\n";
                    break;
                case '\r':
                    ctx.out << "\\r";
                    break;
                case '"':
                    ctx.out << "\\\"";
                    break;
                case '\\':
                    ctx.out << "\\\\";
                    break;
                default:
                    ctx.out << sym;
            }
        }
        ctx.out << '"';
    }

    void Node::PrintValue(std::nullptr_t, const PrintContext& ctx) const {
        ctx.out << "null";
    }

    void Node::PrintValue(const double &value, const PrintContext& ctx) const {
        ctx.out << value;
    }

    void Node::PrintValue(const Array &value, const PrintContext& ctx) const {
        ctx.out << '[' << std::endl;
        bool is_first = true;
        for (const auto &i : value) {
            if (is_first) {
                is_first = false;
            }
            else {
                ctx.out << "," << std::endl;
            }
            ctx.PrintIndent();
            std::visit(
                    [&ctx, this](const auto& value){ PrintValue(value, ctx); },
                    i.GetValue());
        }
        ctx.out << std::endl;
        ctx.out << ']';
    }

    void Node::PrintValue(const Dict &value, const PrintContext& ctx) const {
        ctx.out << '{' << std::endl;
        bool is_first = true;
        for (const auto &[k, v] : value) {
            if (is_first) {
                is_first = false;
            }
            else {
                ctx.out << "," << std::endl;
            }
            ctx.PrintIndent();
            ctx.out << '\"' << k << "\" : ";
            std::visit(
                    [&ctx, this](const auto& value){ PrintValue(value, ctx.Indented()); },
                    v.GetValue());
        }
        ctx.out << std::endl << '}';
    }

    void Node::PrintNode(const json::Node::PrintContext &ctx) const {
        std::visit(
                [&ctx, this](const auto& value){ PrintValue(value, ctx.Indented()); },
                data_);
    }
    Node &Node::operator=(const Node &other) {
        data_ = other.data_;
        return *this;
    }

    void Node::PrintContext::PrintIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }
}  // namespace json