local _modules = {
    {
        name = "ludo",
        version = "0.0.1-dev",
        dependencies = {
            "licht.engine.core",
            "licht.engine.rhi",
            "licht.engine.rhi.vulkan"
        }
    }
}

return {
    modules = _modules
}
