#include "trie.h"
#include <queue>

// Helper to write a string to a binary stream
void write_string(std::ofstream& out, const std::string& s) {
    size_t len = s.length();
    out.write(reinterpret_cast<const char*>(&len), sizeof(len));
    out.write(s.c_str(), len);
}

// Helper to read a string from a binary stream
std::string read_string(std::ifstream& in) {
    size_t len;
    in.read(reinterpret_cast<char*>(&len), sizeof(len));
    std::string s(len, '\0');
    in.read(&s[0], len);
    return s;
}

TrieNode::TrieNode() : is_leaf(false) {}

TrieNode::~TrieNode() {
    for (auto& pair : children) {
        delete pair.second;
    }
}

bool TrieNode::check_leaf() {
    return is_leaf;
}

void TrieNode::set_leaf(bool leaf) {
    is_leaf = leaf;
}

void TrieNode::set_file_info(const FileInfo& info) {
    file_info = info;
}

FileInfo TrieNode::get_file_info() {
    return file_info;
}

bool TrieNode::has_child(char c) {
    return children.find(c) != children.end();
}

TrieNode* TrieNode::get_child(char c) {
    if (has_child(c)) {
        return children[c];
    }
    return nullptr;
}

TrieNode* TrieNode::add_child(char c) {
    if (!has_child(c)) {
        children[c] = new TrieNode();
    }
    return children[c];
}

std::unordered_map<char, TrieNode*>& TrieNode::get_children() {
    return children;
}

TrieSearch::TrieSearch() {
    root = new TrieNode();
}

TrieSearch::~TrieSearch() {
    delete root;
}

void TrieSearch::insert(const std::string& filename, const std::string& absolute_path, const std::string& extension) {
    TrieNode* current = root;

    for (char c : filename) {
        current = current->add_child(c);
    }

    current->set_leaf(true);
    current->set_file_info(FileInfo(filename, absolute_path, extension));
}

bool TrieSearch::search(const std::string& filename) {
    TrieNode* current = root;

    for (char c : filename) {
        current = current->get_child(c);
        if (current == nullptr) {
            return false;
        }
    }

    return current->check_leaf();
}

std::vector<FileInfo> TrieSearch::search_prefix(const std::string& prefix) {
    std::vector<FileInfo> results;
    TrieNode* current = root;

    for (char c : prefix) {
        current = current->get_child(c);
        if (current == nullptr) {
            return results;
        }
    }

    collect_all_files(current, prefix, results);

    return results;
}

std::vector<FileInfo> TrieSearch::search_prefix_n_results(const std::string& prefix, int num_results) {
    std::vector<FileInfo> results;
    TrieNode* current = root;

    for (char c : prefix) {
        current = current->get_child(c);
        if (current == nullptr) {
            return results;
        }
    }

    collect_n_files(current, prefix, results, num_results);

    return results;
}

void TrieSearch::collect_n_files(TrieNode *node, const std::string& prefix, std::vector<FileInfo> &results, int n) {
    if (n <= 0) return;
    std::queue<std::pair<TrieNode*, std::string>> q;
    q.emplace(node, prefix);

    while (!q.empty() && results.size() < n) {
        auto [current, path] = q.front();
        q.pop();

        if (current->check_leaf()) {
            results.push_back(current->get_file_info());
            if (results.size() >= n) {
                return;
            }
        }

        for (auto& pair : current->get_children()) {
            q.emplace(pair.second, path + pair.first);
        }
    }
}

void TrieSearch::collect_all_files(TrieNode* node, std::string prefix, std::vector<FileInfo>& results) {
    if (node->check_leaf()) {
        results.push_back(node->get_file_info());
    }

    for (auto& pair : node->get_children()) {
        collect_all_files(pair.second, prefix + pair.first, results);
    }
}

bool TrieSearch::remove(const std::string& filename) {
    return remove_helper(root, filename, 0);
}

bool TrieSearch::remove_helper(TrieNode* node, const std::string& filename, int depth) {
    if (node == nullptr) {
        return false;
    }

    if (depth == filename.length()) {
        if (!node->check_leaf()) {
            return false;
        }

        node->set_leaf(false);
        return node->get_children().empty();
    }

    char c = filename[depth];
    TrieNode* child = node->get_child(c);

    if (remove_helper(child, filename, depth + 1)) {
        node->get_children().erase(c);
        delete child;
        return !node->check_leaf() && node->get_children().empty();
    }
    return false;
}

void TrieSearch::save(const std::string& filename) {
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }
    save_node(root, out);
}

void TrieSearch::load(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) {
        std::cerr << "Error opening file for reading: " << filename << std::endl;
        return;
    }
    delete root;
    root = load_node(in);
}

void TrieSearch::save_node(TrieNode* node, std::ofstream& out) {
    bool is_leaf = node->check_leaf();
    out.write(reinterpret_cast<const char*>(&is_leaf), sizeof(is_leaf));

    if (is_leaf) {
        FileInfo info = node->get_file_info();
        write_string(out, info.filename);
        write_string(out, info.absolute_path);
        write_string(out, info.extension);
    }

    size_t num_children = node->get_children().size();
    out.write(reinterpret_cast<const char*>(&num_children), sizeof(num_children));

    for (auto const& [key, val] : node->get_children()) {
        out.write(reinterpret_cast<const char*>(&key), sizeof(key));
        save_node(val, out);
    }
}

TrieNode* TrieSearch::load_node(std::ifstream& in) {
    TrieNode* node = new TrieNode();
    bool is_leaf;
    in.read(reinterpret_cast<char*>(&is_leaf), sizeof(is_leaf));
    node->set_leaf(is_leaf);

    if (is_leaf) {
        std::string filename = read_string(in);
        std::string absolute_path = read_string(in);
        std::string extension = read_string(in);
        node->set_file_info(FileInfo(filename, absolute_path, extension));
    }

    size_t num_children;
    in.read(reinterpret_cast<char*>(&num_children), sizeof(num_children));


    for (size_t i = 0; i < num_children; ++i) {
        char key;
        in.read(reinterpret_cast<char*>(&key), sizeof(key));
        TrieNode* child = load_node(in);
        node->get_children()[key] = child;
    }

    return node;
}
