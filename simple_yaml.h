#ifndef SIMPLE_YAML_H
#define SIMPLE_YAML_H

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace simple_yaml {

enum class NodeType { Null, Scalar, Sequence, Map };

class Node {
public:
    Node() : type_(NodeType::Null) {}

    static Node create_scalar(const std::string& val) {
        Node n; n.type_ = NodeType::Scalar; n.scalar_value_ = val; return n;
    }
    static Node create_sequence() {
        Node n; n.type_ = NodeType::Sequence; return n;
    }
    static Node create_map() {
        Node n; n.type_ = NodeType::Map; return n;
    }

    NodeType Type() const { return type_; }
    bool IsNull()     const { return type_ == NodeType::Null; }
    bool IsScalar()   const { return type_ == NodeType::Scalar; }
    bool IsSequence() const { return type_ == NodeType::Sequence; }
    bool IsMap()      const { return type_ == NodeType::Map; }

    explicit operator bool() const { return type_ != NodeType::Null; }

    std::string Scalar() const {
        if (type_ != NodeType::Scalar) throw std::runtime_error("not a scalar");
        return scalar_value_;
    }

    template<typename T> T as() const;

    size_t size() const {
        if (type_ == NodeType::Sequence) return seq_items_.size();
        if (type_ == NodeType::Map) return map_entries_.size();
        return 0;
    }

    const Node& operator[](size_t idx) const {
        static const Node null_node;
        if (type_ != NodeType::Sequence) return null_node;
        return idx < seq_items_.size() ? seq_items_[idx] : null_node;
    }
    Node& operator[](size_t idx) {
        if (type_ != NodeType::Sequence)
            throw std::runtime_error("not a sequence");
        return seq_items_[idx];
    }

    bool has(const std::string& key) const {
        if (type_ != NodeType::Map) return false;
        for (auto& [k, v] : map_entries_) if (k == key) return true;
        return false;
    }

    const Node& operator[](const std::string& key) const {
        static const Node null_node;
        if (type_ != NodeType::Map) return null_node;
        for (auto& [k, v] : map_entries_) if (k == key) return v;
        return null_node;
    }

    Node& operator[](const std::string& key) {
        if (type_ == NodeType::Null) type_ = NodeType::Map;
        if (type_ != NodeType::Map) throw std::runtime_error("not a map");
        for (auto& [k, v] : map_entries_) if (k == key) return v;
        map_entries_.emplace_back(key, Node());
        return map_entries_.back().second;
    }

    using MapEntry = std::pair<std::string, Node>;

    const std::vector<MapEntry>& map_items() const {
        return map_entries_;
    }
    const std::vector<Node>& seq_items() const {
        return seq_items_;
    }

    void push_back(const Node& child) {
        if (type_ == NodeType::Null) type_ = NodeType::Sequence;
        if (type_ != NodeType::Sequence) throw std::runtime_error("not a sequence");
        seq_items_.push_back(child);
    }

    Node clone() const {
        Node n;
        n.type_ = type_;
        n.scalar_value_ = scalar_value_;
        n.seq_items_.reserve(seq_items_.size());
        for (auto& item : seq_items_) n.seq_items_.push_back(item.clone());
        n.map_entries_.reserve(map_entries_.size());
        for (auto& [k, v] : map_entries_) n.map_entries_.emplace_back(k, v.clone());
        return n;
    }

private:
    NodeType type_ = NodeType::Null;
    std::string scalar_value_;
    std::vector<Node> seq_items_;
    std::vector<MapEntry> map_entries_;

    friend Node LoadFile(const std::string& filename);
    friend Node ParseString(const std::string& content);
};

template<> inline std::string Node::as<std::string>() const { return Scalar(); }
template<> inline int Node::as<int>() const { return std::stoi(Scalar()); }
template<> inline double Node::as<double>() const { return std::stod(Scalar()); }
template<> inline bool Node::as<bool>() const {
    auto s = Scalar(); std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s == "true" || s == "yes" || s == "on";
}

namespace detail {

static std::string trim(const std::string& s) {
    size_t a = 0, b = s.size();
    while (a < b && std::isspace((unsigned char)s[a])) a++;
    while (b > a && std::isspace((unsigned char)s[b-1])) b--;
    return s.substr(a, b - a);
}

static std::string ltrim_copy(const std::string& s) {
    size_t a = 0;
    while (a < s.size() && std::isspace((unsigned char)s[a])) a++;
    return s.substr(a);
}

static std::string unquote(const std::string& s) {
    std::string t = trim(s);
    if (t.size() >= 2 && (t[0] == '"' || t[0] == '\'') && t.back() == t[0]) {
        if (t[0] == '"') {
            std::string inner = t.substr(1, t.size() - 2);
            size_t p = 0;
            while ((p = inner.find('\\', p)) != std::string::npos && p + 1 < inner.size()) {
                inner.erase(p, 1);
                p++;
            }
            return inner;
        }
        return t.substr(1, t.size() - 2);
    }
    return t;
}

static int indent_of(const std::string& line) {
    int n = 0;
    for (char c : line) {
        if (c == ' ') n++;
        else if (c == '\t') n += 2;
        else break;
    }
    return n;
}

static bool is_quote(char c) { return c == '"' || c == '\''; }

// Read a scalar string, handling quotes, stopping at any char in terminators
static std::string read_scalar(const std::string& str, size_t& pos, const std::string& terminators) {
    while (pos < str.size() && std::isspace((unsigned char)str[pos])) pos++;
    if (pos >= str.size()) return "";

    // quoted string
    if (str[pos] == '"') {
        pos++;
        std::string result;
        while (pos < str.size() && str[pos] != '"') {
            if (str[pos] == '\\' && pos + 1 < str.size()) {
                pos++; result += str[pos++];
            } else {
                result += str[pos++];
            }
        }
        if (pos < str.size()) pos++; // skip closing quote
        return result;
    }
    if (str[pos] == '\'') {
        pos++;
        std::string result;
        while (pos < str.size() && str[pos] != '\'') {
            result += str[pos++];
        }
        if (pos < str.size()) pos++; // skip closing quote
        return result;
    }

    // unquoted scalar
    size_t start = pos;
    while (pos < str.size() && terminators.find(str[pos]) == std::string::npos
           && !std::isspace((unsigned char)str[pos])) {
        pos++;
    }
    return str.substr(start, pos - start);
}

// Flow value parser forward declaration
static Node parse_flow_value(const std::string& str, size_t& pos);

static Node parse_flow_map(const std::string& str, size_t& pos) {
    if (pos >= str.size() || str[pos] != '{')
        throw std::runtime_error("expected '{'");
    pos++; // skip {
    Node node = Node::create_map();
    while (pos < str.size() && str[pos] != '}') {
        std::string key = read_scalar(str, pos, ":");

        while (pos < str.size() && std::isspace((unsigned char)str[pos])) pos++;
        if (pos < str.size() && str[pos] == ':') pos++; // skip colon
        while (pos < str.size() && std::isspace((unsigned char)str[pos])) pos++;

        Node val = parse_flow_value(str, pos);
        node[key] = val;

        while (pos < str.size() && std::isspace((unsigned char)str[pos])) pos++;
        if (pos < str.size() && str[pos] == ',') {
            pos++; // skip comma
            while (pos < str.size() && std::isspace((unsigned char)str[pos])) pos++;
        }
    }
    if (pos < str.size()) pos++; // skip }
    return node;
}

static Node parse_flow_seq(const std::string& str, size_t& pos) {
    if (pos >= str.size() || str[pos] != '[')
        throw std::runtime_error("expected '['");
    pos++; // skip [
    Node node = Node::create_sequence();
    while (pos < str.size() && str[pos] != ']') {
        Node val = parse_flow_value(str, pos);
        node.push_back(val);

        while (pos < str.size() && std::isspace((unsigned char)str[pos])) pos++;
        if (pos < str.size() && str[pos] == ',') {
            pos++;
            while (pos < str.size() && std::isspace((unsigned char)str[pos])) pos++;
        }
    }
    if (pos < str.size()) pos++; // skip ]
    return node;
}

static Node parse_flow_value(const std::string& str, size_t& pos) {
    while (pos < str.size() && std::isspace((unsigned char)str[pos])) pos++;
    if (pos >= str.size()) return Node::create_scalar("");
    if (str[pos] == '{') return parse_flow_map(str, pos);
    if (str[pos] == '[') return parse_flow_seq(str, pos);
    std::string val = read_scalar(str, pos, ",}]");
    return Node::create_scalar(trim(val));
}

// Block-level parsing

struct ParseResult { Node node; int next; };

static ParseResult parse_value(const std::vector<std::string>& lines, int start, int base_indent);

static ParseResult parse_sequence(const std::vector<std::string>& lines, int start, int seq_indent) {
    Node seq = Node::create_sequence();
    int i = start;
    int content_indent = seq_indent + 2;

    while (i < (int)lines.size()) {
        int line_indent = indent_of(lines[i]);
        std::string raw = ltrim_copy(lines[i]);

        if (line_indent != seq_indent) break;
        if (raw.size() < 2 || raw[0] != '-' || raw[1] != ' ') break;

        std::string rest = trim(raw.substr(2));

        if (rest.empty()) {
            auto [val, next] = parse_value(lines, i + 1, content_indent);
            seq.push_back(val);
            i = next;
        } else if (rest[0] == '{' || rest[0] == '[') {
            size_t pos = 0;
            seq.push_back(parse_flow_value(rest, pos));
            i++;
        } else {
            // Check if it has a colon → inline map entry (start of a block map)
            size_t colon = rest.find(':');
            if (colon != std::string::npos && colon > 0) {
                // Check if colon is inside quotes
                bool in_quote = false;
                char qchar = 0;
                bool colon_is_key_sep = true;
                for (size_t ci = 0; ci < colon; ci++) {
                    if (is_quote(rest[ci])) {
                        if (!in_quote) { in_quote = true; qchar = rest[ci]; }
                        else if (rest[ci] == qchar) { in_quote = false; }
                    }
                }
                if (!in_quote && colon_is_key_sep) {
                    // Inline rest is a map key. Build virtual map starting at content_indent
                    std::vector<std::string> virtual_lines;
                    virtual_lines.push_back(std::string(content_indent, ' ') + rest);

                    int j = i + 1;
                    while (j < (int)lines.size()) {
                        int ni = indent_of(lines[j]);
                        if (ni < content_indent) break;
                        std::string nc = ltrim_copy(lines[j]);
                        if (ni == seq_indent && nc.size() >= 2 && nc[0] == '-' && nc[1] == ' ') break;
                        virtual_lines.push_back(lines[j]);
                        j++;
                    }

                    auto [val, _] = parse_value(virtual_lines, 0, content_indent);
                    seq.push_back(val);
                    i = j;
                    continue;
                }
            }
            seq.push_back(Node::create_scalar(unquote(rest)));
            i++;
        }
    }
    return {seq, i};
}

static ParseResult parse_map(const std::vector<std::string>& lines, int start, int map_indent) {
    Node map_node = Node::create_map();
    int i = start;

    while (i < (int)lines.size()) {
        int line_indent = indent_of(lines[i]);
        std::string raw = ltrim_copy(lines[i]);

        if (line_indent != map_indent) break;

        size_t colon = raw.find(':');
        if (colon == std::string::npos) break;

        // Check colon not inside quotes
        bool in_quote = false;
        char qchar = 0;
        bool colon_valid = true;
        for (size_t ci = 0; ci < colon; ci++) {
            if (is_quote(raw[ci])) {
                if (!in_quote) { in_quote = true; qchar = raw[ci]; }
                else if (raw[ci] == qchar) { in_quote = false; }
            }
        }
        if (in_quote) { colon_valid = false; break; }
        if (!colon_valid) break;

        std::string key = trim(raw.substr(0, colon));
        std::string rest = trim(raw.substr(colon + 1));

        if (rest.empty()) {
            auto [val, next] = parse_value(lines, i + 1, map_indent + 2);
            map_node[key] = val;
            i = next;
        } else if (rest[0] == '{' || rest[0] == '[') {
            size_t pos = 0;
            map_node[key] = parse_flow_value(rest, pos);
            i++;
        } else {
            map_node[key] = Node::create_scalar(unquote(rest));
            i++;
        }
    }
    return {map_node, i};
}

static ParseResult parse_value(const std::vector<std::string>& lines, int start, int base_indent) {
    if (start >= (int)lines.size()) return {Node(), start};

    int indent = indent_of(lines[start]);
    if (indent < base_indent) return {Node(), start};

    std::string raw = ltrim_copy(lines[start]);

    if (raw.empty()) return {Node(), start + 1};

    // Sequence
    if (raw.size() >= 2 && raw[0] == '-' && raw[1] == ' ') {
        return parse_sequence(lines, start, indent);
    }

    // Flow value on its own line (rare in block context, but handle it)
    if (raw[0] == '{' || raw[0] == '[') {
        size_t pos = 0;
        return {parse_flow_value(raw, pos), start + 1};
    }

    // Map entry
    size_t colon = raw.find(':');
    if (colon != std::string::npos && colon > 0) {
        // Verify colon isn't inside quotes
        bool in_quote = false;
        char qchar = 0;
        for (size_t ci = 0; ci < colon; ci++) {
            if (is_quote(raw[ci])) {
                if (!in_quote) { in_quote = true; qchar = raw[ci]; }
                else if (raw[ci] == qchar) { in_quote = false; }
            }
        }
        if (!in_quote) {
            return parse_map(lines, start, indent);
        }
    }

    // Plain scalar
    return {Node::create_scalar(unquote(raw)), start + 1};
}

static std::vector<std::string> preprocess(const std::string& content) {
    std::vector<std::string> lines;
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#') continue;
        lines.push_back(line);
    }
    return lines;
}

} // namespace detail

inline Node ParseString(const std::string& content) {
    auto lines = detail::preprocess(content);
    if (lines.empty()) return Node();
    auto [root, _] = detail::parse_value(lines, 0, 0);
    return root;
}

inline Node LoadFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Cannot open file: " + filename);
    std::stringstream ss;
    ss << file.rdbuf();
    return ParseString(ss.str());
}

} // namespace simple_yaml

#endif
