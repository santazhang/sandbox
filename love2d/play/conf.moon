love.conf = (t) ->
    t.version = "0.9.2"  -- We need LOVE v0.9.2 to run

    t.window.title = "Hello world"
    t.window.width = 800
    t.window.height = 600

    -- Disable unused modules for faster startup
    t.modules.audio = false
    t.modules.sound = false
    t.modules.joystick = false
    t.modules.thread = false
    t.modules.timer = false
