#!/usr/bin/env ruby

require 'simple-rpc.rb'

s = Server.new

s.def 'add' do |*args|
  args.reduce {|a, b| a + b}
end

s.def 'mul' do |*args|
  args.reduce {|a, b| a * b}
end

s.def 'min' do |*args|
  args.reduce {|a, b| a < b ? a : b}
end

s.def 'max' do |*args|
  args.reduce {|a, b| a > b ? a : b}
end

s.start '0.0.0.0', 1987

