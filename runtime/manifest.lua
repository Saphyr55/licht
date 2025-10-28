local _modules = {
    {
        name = "licht.core",
        version = "0.0.1-dev",
        dependencies = {}
    },
    {
        name = "licht.rhi",
        version = "0.0.1-dev",
        dependencies = {
            "licht.core"
        }
    },
    {
        name = "licht.rhi.vulkan",
        version = "0.0.1-dev",
        dependencies = {
            "licht.core",
            "licht.rhi"
        }
    },
    {
        name = "licht.renderer",
        version = "0.0.1-dev",
        dependencies = {
            "licht.core",
            "licht.engine",
            "licht.rhi"
        }
    },
    {
        name = "licht.engine",
        version = "0.0.1-dev",
        dependencies = {
            "licht.core"
        }
    },
    {
        name = "licht.scene",
        version = "0.0.1-dev",
        dependencies = {
            "licht.core",
            "licht.engine",
            "licht.renderer",
            "licht.rhi"
        }
    },
    {
        name = "licht.launcher",
        version = "0.0.1-dev",
        dependencies = {
            "licht.core",
            "licht.engine"
        }
    },
    {
        name = "licht.messaging",
        version = "0.0.1-dev",
        dependencies = {
            "licht.core",
            "licht.engine"
        }
    }
}

return {
    modules = _modules
}
