local debug = true
love.load = function(arg) end
love.update = function(dt)
  if love.keyboard.isDown('escape') then
    local r = love.window.showMessageBox('Quit?', 'Are you sure?', {
      'YES',
      'NO'
    })
    if r == 1 then
      return love.event.push('quit')
    end
  elseif love.keyboard.isDown('f') then
    local fullscreen, _ = love.window.getFullscreen()
    return love.window.setFullscreen((not fullscreen), 'desktop')
  end
end
love.draw = function()
  return love.graphics.print("Hello world!", 400, 300)
end
