# Largest prime factor
#
# The prime factors of 13195 are 5, 7, 13 and 29.
#
# What is the largest prime factor of the number 600851475143?

require 'set'

upper_bound = 7000
primes = []
s = 2.upto(upper_bound).to_a
while !s.empty?
  p = s[0]
  primes << p
  s = s[1..-1].select { |v| v % p != 0 }
end

x = 600851475143

factors = Set.new

primes.each do |p|
  while x % p == 0
    x /= p
    factors.add p
  end
end

if x == 1
  puts factors.max
else
  puts "increase primes upper bound"
end
