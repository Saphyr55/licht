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

add_requires("libsdl3", "catch2", "lua")

function target_files_default(param)
    add_includedirs("include", param)

    add_headerfiles("include/**.hpp")
    add_files("source/**.cpp")
end

-- Engine sources --
includes("runtime/core")
includes("runtime/messaging")
includes("runtime/engine")
includes("runtime/launcher")
includes("runtime/rhi")
includes("runtime/rhi-vulkan")
includes("runtime/scene")
includes("runtime/renderer")
includes("runtime/entity")

-- Sample sources --
includes("samples/ludo/ludo")
includes("samples/ludo/app")

-- Third Party -- 
includes("third_party")
