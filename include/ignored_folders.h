//
// Created by a7x on 30/11/2025.
//

#ifndef SPOTLIGHT_IGNORED_FOLDERS_H
#define SPOTLIGHT_IGNORED_FOLDERS_H
#include <string>
#include <unordered_set>

static std::unordered_set<std::string> ignored_folders_list = {
    "node_modules",
    "build",
    ".git",
    "R"
};


#endif //SPOTLIGHT_IGNORED_FOLDERS_H