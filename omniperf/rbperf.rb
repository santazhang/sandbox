#!/usr/bin/env ruby

def pretty_int val
  val = val.to_i
  val.to_s.gsub(/(\d)(?=(\d\d\d)+(?!\d))/, "\\1,")
end

start = Time.now.to_f
now = Time.now.to_f
ctr = 0
loop do
  now = Time.now.to_f
  break if now - start > 1.0
  target = ctr + 100000
  while ctr < target do
    ctr += 1
  end
end

puts "int++ #{ pretty_int (ctr / (now - start)) }/s"
