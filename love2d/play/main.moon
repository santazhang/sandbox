debug = true

love.load = (arg) ->

love.update = (dt) ->
    if love.keyboard.isDown 'escape'
        r = love.window.showMessageBox 'Quit?', 'Are you sure?', {'YES', 'NO'}
        if r == 1
            love.event.push 'quit'

    elseif love.keyboard.isDown 'f'
        fullscreen, _ = love.window.getFullscreen!
        love.window.setFullscreen (not fullscreen), 'desktop'

love.draw = ->
    love.graphics.print "Hello world!", 400, 300
