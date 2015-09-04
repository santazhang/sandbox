def pretty_int val
  val = val.to_i
end

start = Time.now
now = Time.now

ctr = 0

loop do
  now = Time.now
  elapsed = now - start
  break if elapsed.total_seconds > 1
  target = ctr + 100000
  loop do
    ctr += 1
    break if ctr > target
  end
end

puts start
puts ctr

puts "int++ #{ pretty_int (ctr / (now - start).total_seconds) }/s"

