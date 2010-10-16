-- This is a script browser
-- Santa Zhang (santa1987@gmail.com), 2010

c_file = Color.new(0, 255, 0)
c_folder = Color.new(0, 0, 255)
c_selected = Color.new(255, 0, 0)
c_white = Color.new(255, 255, 255)

last_dir = ""
cur_dir = "../Application"

function sorted_ls(dir)
  files = System.listDirectory(dir)
  table.sort(files, function (a, b) return a.name < b.name end)
  return files
end


function table_size(tab)
  size = 0
  for index, file in ipairs(tab) do
    size = size + 1
  end
  return size
end

last_btn = Controls.read()

info = "Press 'O' to enter folder or boot .lua scripts"

while true do
  screen:clear()
  if cur_dir ~= last_dir then
    System.currentDirectory(cur_dir)
    cur_dir = System.currentDirectory()
    files = sorted_ls(".")
    last_dir = cur_dir
    cur_idx = 1
    top_idx = 1
  end
  screen:print(0, 0, info, c_white)
  screen:print(0, 8, "CWD: "..cur_dir, c_white)
  screen:drawLine(0, 18, 479, 18, c_white)
  for index, file in ipairs(files) do
    if top_idx <= index and index <= top_idx + 30 then
      if index == cur_idx then
        screen:print(0, (index - top_idx) * 8 + 20, file.name, c_selected)
      elseif file.directory then
        screen:print(0, (index - top_idx) * 8 + 20, file.name, c_folder)
      else
        screen:print(0, (index - top_idx) * 8 + 20, file.name, c_file)
      end
    end
  end
  btn = Controls.read()
  if btn:up() and btn ~= last_btn then
    if cur_idx > 1 then
      cur_idx = cur_idx - 1
    end
  elseif btn:down() and btn ~= last_btn then
    if cur_idx < table_size(files) then
      cur_idx = cur_idx + 1
    end
  elseif btn:circle() and btn ~= last_btn then
    if files[cur_idx].directory then
      cur_dir = cur_dir.."/"..files[cur_idx].name
    else
      dofile(files[cur_idx].name)
    end
  end
  
  if top_idx > cur_idx then
    top_idx = cur_idx
  elseif top_idx + 30 < cur_idx then
    top_idx = cur_idx - 30
  end
  
  last_btn = btn
  screen:waitVblankStart()
  screen:flip()
end
