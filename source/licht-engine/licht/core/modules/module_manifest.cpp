#include "licht/core/modules/module_manifest.hpp"

#include "licht/core/string/format.hpp"
#include "licht/core/trace/trace.hpp"

#include <lua.hpp>

namespace licht {

static bool module_manifest_information_lua_validate(lua_State* L, int index) {
    bool valid = true;

    lua_getfield(L, index, ModuleManifestKeyNames::Name);
    if (!lua_isstring(L, -1)) {
        valid = false;
    }
    lua_pop(L, 1);

    lua_getfield(L, index, ModuleManifestKeyNames::Version);
    if (!lua_isstring(L, -1)) {
        valid = false;
    }
    lua_pop(L, 1);

    lua_getfield(L, index, ModuleManifestKeyNames::Dependencies);
    if (!lua_istable(L, -1) && !lua_isnil(L, -1)) {
        StringRef message = lua_tostring(L, -2);
        LLOG_ERROR("[ModuleManifest]", vformat("Module '%s' ' has invalid 'dependencies', must be a table.", message))
        valid = false;
    }
    lua_pop(L, 1);

    return valid;
}

static ModuleManifestInformation module_manifest_information_lua_parse(lua_State* L, int32 table_index) {
    ModuleManifestInformation description;

    lua_getfield(L, -1, ModuleManifestKeyNames::Name);
    if (lua_isstring(L, -1)) {
        description.name = lua_tostring(L, -1); 
    } else {
        LLOG_ERROR("[ModuleManifest]", "Module is missing a valid 'name' field.");
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, ModuleManifestKeyNames::Version);
    if (lua_isstring(L, -1)) {
        description.version = lua_tostring(L, -1);
    } else {
        LLOG_ERROR("[ModuleManifest]", vformat("Module '%s' is missing a valid 'version' field.", description.name.data()));
    }
    lua_pop(L, 1);
    
    lua_getfield(L, -1, ModuleManifestKeyNames::Dependencies);
    if (lua_istable(L, -1)) {
        
        usize len = lua_rawlen(L, -1);
        for (usize i = 1; i <= len; i++) {
            
            lua_rawgeti(L, -1, i);
            if (lua_isstring(L, -1)) {
                description.dependencies.push_back(lua_tostring(L, -1));
            } else {
                LLOG_ERROR("[ModuleManifest]",  vformat("Module '%s' has a non-string dependency at index %zu.", description.name.data(), i));
            }
            lua_pop(L, 1); 
            
        }
        
    } else {
        LLOG_ERROR("[ModuleManifest]", vformat("Module '%s' ' has invalid 'dependencies', must be a table.", description.name.data()))
    }

    lua_pop(L, 1);

    return description;
}

bool ModuleManifest::load_from_lua_manifest(StringRef filepath) {

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    if (luaL_dofile(L, filepath.data()) != LUA_OK) {
        lua_close(L);
        LLOG_ERROR("[ModuleManifest]", vformat("Failed to load Lua file: ", filepath));
        return false;
    }

    if (!lua_istable(L, -1)) {
        lua_close(L);
        LLOG_ERROR("[ModuleManifest]", "Lua manifest must return a table.");
        return false;
    }

    int32 table_index = lua_gettop(L);
    lua_pushnil(L);

    while (lua_next(L, table_index) != 0) {
        if (lua_istable(L, -1)) {
            manifest_informations_.append(module_manifest_information_lua_parse(L, table_index));
        }
        lua_pop(L, 1);
    }

    lua_pop(L, 1);
    lua_close(L);

    return true;
}

void log_module_manifest(const ModuleManifest& manifest) {

    const Array<ModuleManifestInformation>& module_informations = manifest.get_manifest_informations();
    LLOG_INFO("[ModuleManifest]", vformat("Module Manifest Informations (%d modules):", module_informations.size()));

    for (usize i = 0; i < manifest.get_manifest_informations().size(); i++) {
        const ModuleManifestInformation& module_information = module_informations[i];
        LLOG_INFO("[ModuleManifest]", vformat("Module:"));
        LLOG_INFO("[ModuleManifest]", vformat("  %s: %s", ModuleManifestKeyNames::Name, module_information.name.data()));
        LLOG_INFO("[ModuleManifest]", vformat("  %s: %s", ModuleManifestKeyNames::Version, module_information.version.data()));

        if (module_information.dependencies.empty()) {
            LLOG_INFO("[ModuleManifest]", vformat("  %s: (empty)", ModuleManifestKeyNames::Dependencies));
            continue;
        }

        usize dependency_count = module_information.dependencies.size();
        String dependency_names(256);
        for (usize j = 0; j < dependency_count; j++) {
            dependency_names.append(module_information.dependencies[j]);
            if (j < dependency_count - 1) {
                dependency_names.append(", ");
            }
        }

        LLOG_INFO("[ModuleManifest]", vformat("  %s: (%s)", ModuleManifestKeyNames::Dependencies, dependency_names.data()));
    }
}

Array<ModuleManifestInformation>& ModuleManifest::get_manifest_informations() {
    return manifest_informations_;
}

const Array<ModuleManifestInformation>& ModuleManifest::get_manifest_informations() const {
    return manifest_informations_;
}

}  //namespace licht