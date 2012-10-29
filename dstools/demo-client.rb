#!/usr/bin/env ruby

require 'simple-rpc.rb'

c = Client.new 'localhost', 1987

f = c.async_add 1, 2, 4, 5

puts f.get

puts c.sync_add 2

f = c.async_add 3, 4 do |cb|
  cb.success do |result|
    puts "from block: result = #{result}"
  end
  cb.failure do |cause|
    puts "from block: failure! #{cause}"
  end
end


f.get


puts "benchmarking"

n = 100000
farr = []
start = Time.now
(1..n).each do |i|
  farr << c.async_max(1, 2, 3, 4, 5, 6, 7, i) do |cb|
    cb.success do |result|
      puts "block result: #{result}" if i % 1000 == 0
    end
    cb.failure do |cause|
      puts "block failure: #{cause}"
    end
  end
end

farr.each {|f| f.get}
stop = Time.now

puts "#{n} req took #{stop - start}"
