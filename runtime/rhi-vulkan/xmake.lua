add_requires("vulkan-headers")

target("licht.rhi.vulkan", function()
    set_kind("shared")
    set_group("engine")

    add_deps("licht.core", "licht.rhi")

    target_files_default({
        public = true
    })

    add_headerfiles("source/**.hpp")
    add_includedirs("source", {
        public = false
    })

    add_packages("vulkan-headers", {
        public = false
    })

    add_defines("LICHT_RHI_VULKAN_EXPORTS")

    if is_plat("windows") then
        add_defines("VK_USE_PLATFORM_WIN32_KHR")
        add_syslinks("User32")
    end
end)
