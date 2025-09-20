set_project("licht")
set_version("0.0.1-dev")
set_languages("c++20")

set_targetdir("build/bin/$(plat)_$(arch)_$(mode)")

add_rules("mode.debug", "mode.release")
add_rules("plugin.vsxmake.autoupdate")
add_rules("plugin.compile_commands.autoupdate", {
    outputdir = ".xmake"
})

if is_mode("debug") then
    add_defines("LDEBUG")
end

add_requires("libsdl3", "catch2", "vulkan-headers", "lua")

includes("scripts/xmake/**.lua")

function target_files_default(param)
    add_includedirs("include", param)

    add_headerfiles("include/**.hpp")
    add_files("source/**.cpp")
end

-- Engine sources --
includes("engine/core")
includes("engine/engine")
includes("engine/launcher")
includes("engine/rhi")
includes("engine/rhi-vulkan")

-- Sample sources --
includes("samples/ludo")
