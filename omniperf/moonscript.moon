i = 0
n = 50 * 1000 * 1000
t1 = os.clock()
while i < n
  i = i + 1
t2 = os.clock()
print("i++", n / (t2 - t1))
