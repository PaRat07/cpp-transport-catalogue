#pragma once

#include <iostream>
#include <sstream>
#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {

    class Node;

    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    // Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node {
    public:
        /* Реализуйте Node, используя std::variant */

        explicit Node() = default;
        Node(const Node &other) = default;
        Node(Array array);
        Node(Dict map);
        Node(int value);
        Node(bool value);
        Node(std::nullptr_t);
        Node(double value);
        Node(std::string_view value);

        // Приводят узел к этому типу данных
        const Array& AsArray() const;
        const Dict& AsMap() const;
        int AsInt() const;
        bool AsBool() const;
        double AsDouble() const;
        const std::string& AsString() const;

        // Возвращаю, является ли этот узел этом типом
        bool IsInt() const;
        bool IsDouble() const;
        bool IsPureDouble() const;
        bool IsBool() const;
        bool IsString() const;
        bool IsNull() const;
        bool IsArray() const;
        bool IsMap() const;

        // Возвращает копию значения узла
        std::variant<std::nullptr_t, int, double, std::string, bool, Array, Dict> GetValue() const;

        bool operator==(const Node &other) const;
        bool operator!=(const Node &other) const;
        Node &operator=(const Node &other);
    private:
// Контекст вывода, хранит ссылку на поток вывода и текущий отсуп
        struct PrintContext {
            std::ostream& out;
            int indent_step = 4;
            int indent = 0;

            void PrintIndent() const;

            // Возвращает новый контекст вывода с увеличенным смещением
            PrintContext Indented() const {
                return {out, indent_step, indent_step + indent};
            }
        };

        // Печатают значение всех возможных типов узла
        void PrintValue(const Dict &value, const PrintContext& ctx) const;
        void PrintValue(const int &value, const PrintContext& ctx) const;
        void PrintValue(const bool &value, const PrintContext& ctx) const;
        void PrintValue(const double &value, const PrintContext& ctx) const;
        void PrintValue(const Array &value, const PrintContext& ctx) const;
        void PrintValue(const std::string &value, const PrintContext &ctx) const;
        void PrintValue(std::nullptr_t, const PrintContext &ctx) const;

        std::variant<std::nullptr_t, int, double, std::string, bool, Array, Dict> data_;
    public:
        void PrintNode(const PrintContext& ctx) const;
    };

    class Document {
    public:
        bool operator==(const Document &other) const {
            return root_ == other.root_;
        }
        bool operator!=(const Document &other) const {
            return root_ != other.root_;
        }
        explicit Document(Node root);

        // Возвращает в сыром виде узел, который хранит
        const Node& GetRoot() const;

    private:
        Node root_;
    };

    // Загружает из потока JSON
    Document Load(std::istream& input);

    // Печатает документ в поток
    void Print(const Document& doc, std::ostream& output);

}  // namespace json