debug = true

love.load = (arg) ->

love.update = (dt) ->
    if love.keyboard.isDown 'escape'
        love.event.push 'quit'

love.draw = ->
    love.graphics.print "Hello world!", 400, 300
