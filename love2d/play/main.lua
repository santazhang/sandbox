local debug = true
love.load = function(arg) end
love.update = function(dt)
  if love.keyboard.isDown('escape') then
    return love.event.push('quit')
  end
end
love.draw = function()
  return love.graphics.print("Hello world!", 400, 300)
end
