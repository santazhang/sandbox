#!/usr/bin/env ruby

require 'rubygems'
require 'sdl'

SDL.init( SDL::INIT_VIDEO ) 
screen = SDL::Screen.open( 640, 480, 16, SDL::SWSURFACE ) 

loop do
  while event = SDL::Event2.poll # (1)
	puts event
    case event # (2)
    when SDL::Event2::Quit 
	puts "quit"
      exit # (3)
    when SDL::Event2::KeyDown
      if event.sym == SDL::Key::SPACE then # (4)
puts "space"
      end
    end
  end

#  screen.fillRect( 0, 0, 640, 480, [ 0, 0, 0 ] )
#  screen.updateRect( 0, 0, 0, 0 )
end
