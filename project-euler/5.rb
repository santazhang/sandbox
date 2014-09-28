# Smallest multiple
#
# 2520 is the smallest number that can be divided by each of the numbers from 1
# to 10 without any remainder.
#
# What is the smallest positive number that is evenly divisible by all of the
# numbers from 1 to 20?

$primes = [2, 3, 5, 7, 11, 13, 17, 19]

def canonical(n)
  v = [0] * $primes.length
  while n != 1
    $primes.each_with_index do |p, idx|
      if n % p == 0
        n /= p
        v[idx] += 1
      end
    end
  end
  v
end

def merge(v1, v2)
  v = v1
  0.upto(v.length - 1).each do |idx|
    if v1[idx] < v2[idx]
      v[idx] = v2[idx]
    end
  end
  v
end

v = canonical(1)
2.upto(20).each do |n|
  v = merge v, canonical(n)
end

u = 1
$primes.each_with_index do |p, idx|
  u *= p ** v[idx]
end
puts u
