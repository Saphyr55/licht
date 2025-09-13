set_project("licht")
set_version("0.0.1-dev")
set_languages("c++20")

set_targetdir("build/bin/$(plat)_$(arch)_$(mode)")

add_rules("mode.debug", "mode.release")
add_rules("plugin.vsxmake.autoupdate")
add_rules("plugin.compile_commands.autoupdate", {outputdir = ".xmake"})

if is_mode("debug") then
    add_defines("LDEBUG")
end

function licht_engine_add_includedirs(includedirs)
    add_includedirs("source/licht-engine")
    if (includedirs == nil) then 
        return
    end
    local dirs = {unpack(includedirs)}
    for _, dir in ipairs(dirs) do
        add_includedirs(dir)
    end
end

includes("scripts/**.lua")

local project_dir = path.absolute("$(projectdir)")
add_defines(format("LICHT_PROJECT_DIR=\"%s\"", project_dir))

add_requires("libsdl3", "catch2", "vulkan-headers", "lua")

target("licht.core")
    set_kind("shared")

    add_packages("libsdl3", "lua")

    licht_engine_add_includedirs()

    add_files("source/licht-engine/licht/core/**.cpp")
    add_headerfiles("source/licht-engine/licht/core/**.hpp")

    add_defines("LICHT_CORE_EXPORTS")


target("licht.rhi")
    set_kind("shared")
    
    add_deps("licht.core")

    licht_engine_add_includedirs()

    add_files("source/licht-engine/licht/rhi/**.cpp")
    add_headerfiles("source/licht-engine/licht/rhi/**.hpp")
    
    add_defines("LICHT_RHI_EXPORTS")


target("licht.rhi.vulkan")
    set_kind("shared")

    add_deps("licht.core")

    licht_engine_add_includedirs()

    add_files("source/licht-engine/licht/rhi_vulkan/**.cpp")
    add_headerfiles("source/licht-engine/licht/rhi_vulkan/**.hpp")

    add_packages("vulkan-headers", { public = true })

    add_defines("LICHT_RHI_VULKAN_EXPORTS")

	if is_plat("windows", "mingw") then
		add_defines("VK_USE_PLATFORM_WIN32_KHR")
		add_syslinks("User32")
    end


target("licht.core.tests")
    set_kind("binary")

    add_deps("licht.core")
    
    add_packages("catch2")

    licht_engine_add_includedirs()
    
    add_files("tests/core/**.cpp")


target("licht.demo")
    set_kind("binary")

    -- TODO: Add licht.rhi.vulkan as a runtime dependency when the demo uses it.
    add_deps("licht.core", "licht.rhi.vulkan")

    licht_engine_add_includedirs("source/licht-demo")

    add_files("source/licht-demo/**.cpp")
    add_headerfiles("source/licht-demo/**.hpp")

    after_build(function (target) 

        import("core.base.task")
		import("core.project.project")

        local output_shaders = path.join(target:targetdir(), "shaders")

        task.run("compile-shaders", { 
            files = {
                "shaders/main.frag",
                "shaders/main.vert"
            }, 
            output = output_shaders
        })

    end)
    