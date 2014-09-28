# 10001st prime
#
# By listing the first six prime numbers: 2, 3, 5, 7, 11, and 13, we can see
# that the 6th prime is 13.
#
# What is the 10 001st prime number?

require 'set'

def nth_prime(nth)
  # http://en.wikipedia.org/wiki/Prime-counting_function
  upper_bound = (2 + 1.26 * nth * Math.log(nth)).to_i

  primes = []
  s = 2.upto(upper_bound).to_a
  while !s.empty?
    p = s[0]
    primes << p
    s = s[1..-1].select { |v| v % p != 0 }
  end
  primes[nth - 1]
end

puts nth_prime(10001)
