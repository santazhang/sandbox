-- Santa Zhang's pspKit
-- Santa Zhang (santa1987@gmail.com), 2010

font = Font.createProportional()
c = Color.new(0, 0, 255)
c2 = Color.new(255, 0, 0)
pwd = ""

-- locks until password "LORX" is given
while true do
  screen:clear()
  btn = Controls.read()
  if btn:start() then
    return
  elseif btn:l() then
    if pwd == "" or pwd == "L" then
      pwd = "L"
    else
      pwd = ""
    end
  elseif btn:circle() then
    if pwd == "L" or pwd == "LO" then
      pwd = "LO"
    else
      pwd = ""
    end
  elseif btn:r() then
    if pwd == "LO" or pwd == "LOR" then
      pwd = "LOR"
    else
      pwd = ""
    end
  elseif btn:cross() then
    if pwd == "LOR" then
      break
    else
      pwd = ""
    end
  end
  screen:print(0, 0, "This is Santa Zhang's pspKit!", c)
  screen:print(0, 8, "Press 'start' to quit.", c)
  screen:waitVblankStart()
  screen:flip()
end

screen:clear()
screen:print(0, 0, "Loading real system...", c2)
screen:waitVblankStart()
screen:flip()

System.currentDirectory("System")
dofile("real_system.lua")

screen:clear()
screen:print(0, 0, "Quitting...", c2)
screen:waitVblankStart()
screen:flip()

return
