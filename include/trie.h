//
    // Created by a7x on 01/12/2025.
    //

    #ifndef SPOTLIGHT_TRIE_H
    #define SPOTLIGHT_TRIE_H

    #include <unordered_map>
    #include <string>
    #include <vector>

#include <iostream>
#include <fstream>

struct FileInfo {
    std::string filename;
    std::string absolute_path;
    std::string extension;

    FileInfo() = default;
    FileInfo(const std::string& name, const std::string& path, const std::string& ext)
        : filename(name), absolute_path(path), extension(ext) {}
};

class TrieNode {
private:
    bool is_leaf;
    FileInfo file_info;
    std::unordered_map<char, TrieNode*> children;

public:
    TrieNode();
    ~TrieNode();

    bool check_leaf();
    void set_leaf(bool leaf);
    void set_file_info(const FileInfo& info);
    FileInfo get_file_info();
    bool has_child(char c);
    TrieNode* get_child(char c);
    TrieNode* add_child(char c);
    std::unordered_map<char, TrieNode*>& get_children();
};

class TrieSearch {
private:
    TrieNode* root;

    void collect_all_files(TrieNode* node, std::string prefix, std::vector<FileInfo>& results);
    void collect_n_files(TrieNode* node, const std::string& prefix, std::vector<FileInfo>& results, int n);
    bool remove_helper(TrieNode* node, const std::string& filename, int depth);
    void save_node(TrieNode* node, std::ofstream& out);
    TrieNode* load_node(std::ifstream& in);

public:
    TrieSearch();
    ~TrieSearch();

    void insert(const std::string& filename, const std::string& absolute_path, const std::string& extension);
    bool search(const std::string& filename);
    std::vector<FileInfo> search_prefix(const std::string& prefix);
    std::vector<FileInfo> search_prefix_n_results(const std::string& prefix, int num_results);
    bool remove(const std::string& filename);
    void save(const std::string& filename);
    void load(const std::string& filename);


};

#endif //SPOTLIGHT_TRIE_H
