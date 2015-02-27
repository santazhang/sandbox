local i = 0
local n = 1000 * 1000 * 50
local t1 = os.clock()
while i < n do
  i = i + 1
end
local t2 = os.clock()
print("i++", n / (t2 - t1))
