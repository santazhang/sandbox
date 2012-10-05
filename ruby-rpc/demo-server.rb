#!/usr/bin/env ruby

require 'server.rb'

s = Server.new

s.def 'add' do |*a|
  require 'pp'
  pp a
end

