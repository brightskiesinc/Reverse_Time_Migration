/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of SeismicToolbox.
 *
 * SeismicToolbox is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SeismicToolbox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <dirent.h>
#include <string>
#include <fstream>

#include <bs/base/api/cpp/BSBase.hpp>

#include <stbx/parsers/Parser.hpp>

#define JSON_EXTENSION ".json"

using namespace std;
using json = nlohmann::json;
using namespace bs::base::logger;
using namespace stbx::parsers;


void Parser::RegisterFile(const std::string &file) {
    this->mFiles.push_back(file);
}

void Parser::RegisterFolder(const std::string &folder) {
    struct dirent *entry;
    DIR *dp = opendir(folder.c_str());
    if (dp != nullptr)
        while ((entry = readdir(dp))) {
            string file_name = entry->d_name;
            if (file_name.find(JSON_EXTENSION) != string::npos) {
                this->mFiles.push_back(folder + file_name);
            }
        }
}

json Parser::BuildMap() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    vector<int> erase;
    string extension = string(JSON_EXTENSION);
    int size = this->mFiles.size();
    Logger->Info() << "The following configuration files were detected : " << '\n';
    for (int i = 0; i < size; i++) {
        Logger->Info() << "\t" << (i + 1) << ". " << this->mFiles[i] << '\n';
        if (this->mFiles[i].substr(this->mFiles[i].size() - extension.size()) != extension) {
            Logger->Info() << "\tRegistered file extension is not supported..." << '\n';
            erase.push_back(i);
        }
    }
    for (int &i:erase) {
        this->mFiles.erase(this->mFiles.begin() + i);
    }

    for (string &f : this->mFiles) {
        std::ifstream in(f.c_str());
        json temp;
        in >> temp;
        this->mMap.merge_patch(temp);
    }
    return this->mMap;
}

json Parser::GetMap() {
    return this->mMap;
}

const vector<std::string> &Parser::GetFiles() const {
    return this->mFiles;
}
