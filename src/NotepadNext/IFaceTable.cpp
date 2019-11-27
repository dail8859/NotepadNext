/*
 * This file is part of Notepad Next.
 * Copyright 2019 Justin Dailey
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Notepad Next is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Notepad Next.  If not, see <https://www.gnu.org/licenses/>.
 */


#include "IFaceTable.h"
#include <string.h>
#include <ctype.h>
#include <algorithm>
#include <iterator>

template<typename Iter>
typename const std::iterator_traits<Iter>::value_type *binary_find(Iter begin, Iter end, const char *name) {
    auto it = std::lower_bound(begin, end, name, [](const auto &lhs, const char *rhs) {
        return strcmp(lhs.name, rhs) < 0;
    });

    if (it == end || strcmp(name, it->name) != 0) {
        return nullptr;
    }
    else {
        return &(*it);
    }
}

const IFaceConstant *IFaceTable::FindConstant(const char *name) const {
    return binary_find(constants.cbegin(), constants.cend(), name);
}

const IFaceFunction *IFaceTable::FindFunction(const char *name) const {
    return binary_find(functions.cbegin(), functions.cend(), name);
}

const IFaceFunction *IFaceTable::FindFunctionByConstantName(const char *name) const {
    if (strncmp(name, prefix, strlen(prefix)) == 0) {
        // This looks like a constant for an iface function.  This requires
        // a sequential search.  Take special care since the function names
        // are mixed case, whereas the constants are all-caps.

        for (const auto &func : functions) {
            const char *nm = name + strlen(prefix);
            const char *fn = func.name;
            while (*nm && *fn && (*nm == toupper(*fn))) {
                ++nm;
                ++fn;
            }
            if (!*nm && !*fn) {
                return &func;
            }
        }
    }
    return nullptr;
}

const IFaceFunction *IFaceTable::FindFunctionByValue(int value) const {
    for (const auto &func : functions) {
        if (func.value == value)
            return &func;
    }
    return nullptr;
}

const IFaceProperty *IFaceTable::FindProperty(const char *name) const {
    return binary_find(properties.cbegin(), properties.cend(), name);
}

int IFaceTable::GetConstantName(int value, char *nameOut, unsigned nameBufferLen, const char *hint) const {
    if (nameOut && nameBufferLen > 0) {
        *nameOut = '\0';
    }

    // Look in both the constants table and the functions table.  Start with functions.
    for (const auto &func : functions) {
        if (func.value == value) {
            int len = static_cast<int>(strlen(func.name) + strlen(prefix));
            if (nameOut && (static_cast<int>(nameBufferLen) > len)) {
                strcpy(nameOut, prefix);
                strcat(nameOut, func.name);
                // fix case
                for (char *nm = nameOut + strlen(prefix); *nm; ++nm) {
                    if (*nm >= 'a' && *nm <= 'z') {
                        *nm = static_cast<char>(*nm - 'a' + 'A');
                    }
                }
                return len;
            } else {
                return -1 - len;
            }
        }
    }

    for (const auto &con : constants) {
        if (con.value == value && (hint == NULL || strncmp(hint, con.name, strlen(hint)) == 0)) {
            int len = static_cast<int>(strlen(con.name));
            if (nameOut && (static_cast<int>(nameBufferLen) > len)) {
                strcpy(nameOut, con.name);
                return len;
            } else {
                return -1 - len;
            }
        }
    }

    return 0;
}

const IFaceFunction *IFaceTable::GetFunctionByMessage(int message) const {
    for (const auto &func : functions) {
        if (func.value == message) {
            return &func;
        }
    }
    return nullptr;
}

IFaceFunction IFaceTable::GetPropertyFuncByMessage(int message) const {
    for (const auto &prop : properties) {
        if (prop.getter == message) {
            return prop.GetterFunction();
        }
        else if (prop.setter == message) {
            return prop.SetterFunction();
        }
    }
    return { "invalid", -1, iface_void, {iface_void, iface_void} };
}

std::vector<std::string> IFaceTable::GetAllConstantNames() const {
    std::vector<std::string> kws;
    std::string prefix_str(prefix);

    kws.reserve(constants.size() + functions.size());

    std::transform(constants.begin(), constants.end(), std::back_inserter(kws), [](const IFaceConstant &ifc) { return ifc.name; });
    std::transform(functions.begin(), functions.end(), std::back_inserter(kws), [&](const IFaceFunction &iff) {
        std::string s = prefix_str + std::string(iff.name);
        std::transform(s.begin(), s.end(), s.begin(), ::toupper);
        return s;
    });

    std::sort(kws.begin(), kws.end());
    return kws;
}

std::vector<std::string> IFaceTable::GetAllFunctionNames() const {
    std::vector<std::string> kws;

    kws.reserve(functions.size());
    std::transform(functions.begin(), functions.end(), std::back_inserter(kws), [](const IFaceFunction &iff) { return iff.name; });

    return kws;
}

std::vector<std::string> IFaceTable::GetAllPropertyNames() const {
    std::vector<std::string> kws;

    kws.reserve(properties.size());
    std::transform(properties.begin(), properties.end(), std::back_inserter(kws), [](const IFaceProperty &ifp) { return ifp.name; });

    return kws;
}
