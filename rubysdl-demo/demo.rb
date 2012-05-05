#!/usr/bin/env ruby

require 'rubygems'
require 'sdl'


SDL.init( SDL::INIT_VIDEO ) 
screen = SDL.setVideoMode( 640, 480, 0, SDL::SWSURFACE ) 
image = SDL::Surface.load( 'my-avatar.png' )
image.setColorKey( SDL::SRCCOLORKEY, image.getPixel(0,0) ) 
image = image.displayFormat 

image_x = 20

loop do
  while event = SDL::Event2.poll # (1)
    case event # (2)
    when SDL::Event2::Quit 
      exit # (3)
    when SDL::Event2::KeyDown
      if event.sym == SDL::Key::SPACE then # (4)
        image_x += 10
      end
    end
  end

  screen.fillRect( 0, 0, 640, 480, [ 0, 0, 0 ] )
  screen.put( image, image_x, 200 )
  screen.updateRect( 0, 0, 0, 0 )
end
