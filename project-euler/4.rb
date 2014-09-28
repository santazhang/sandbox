# Largest palindrome product
#
# A palindromic number reads the same both ways. The largest palindrome made
# from the product of two 2-digit numbers is 9009 = 91 * 99.
#
# Find the largest palindrome made from the product of two 3-digit numbers.

def is_palindromic(num)
  s = num.to_s
  s == s.reverse
end

palindrome = []
999.downto(100).each do |a|
  a.downto(100).each do |b|
    prod = a * b
    if is_palindromic(prod)
      palindrome << prod
    end
  end
end
puts palindrome.max
