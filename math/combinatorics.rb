
$_fibonacci_cache = [0, 1]

def fibonacci(n)
  return $_fibonacci_cache[n] if $_fibonacci_cache.length > n
  loop do
    idx = $_fibonacci_cache.length
    $_fibonacci_cache << ($_fibonacci_cache[idx - 1] + $_fibonacci_cache[idx - 2])
    break if idx == n
  end
  return $_fibonacci_cache[n]
end

