#!/usr/bin/env ruby

require 'client.rb'

c = Client.new

f = c.async_add 1, 2, 4, 5

puts f.get

puts c.sync_add 2

c.async_add 3, 4 do |ok, result|
end

c.no_such_method

