#!/usr/bin/env ruby

require 'simple-rpc.rb'

c = Client.new 'localhost', 1987

f = c.async_add 1, 2, 4, 5

puts f.get

puts c.sync_add 2

f = c.async_add 3, 4 do |result, failure|
  puts "from block: result = #{result}"
end


f.get

puts "benchmarking"


n = 100
farr = []
start = Time.now
(1..n).each do |i|
  farr << c.async_max(1, 2, 3, 4, 5, 6, 7, i) do |result, failure|
    if failure == nil
      puts "block result: #{result}"
    else
      puts "block failure: #{failure}"
    end
  end
end

farr.each {|f| f.get}
stop = Time.now
sleep 0.1

puts "#{n} req took #{stop - start}"
