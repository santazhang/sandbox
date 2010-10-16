-- MoeBooru picture browser for PSP
-- Santa Zhang (santa1987@gmail.com), 2010

c_file = Color.new(0, 255, 0)
c_folder = Color.new(0, 0, 255)
c_selected = Color.new(255, 0, 0)
c_white = Color.new(255, 255, 255)
c_info = Color.new(255, 0, 255)

last_dir = ""
cur_dir = "."

function table_size(tab)
  size = 0
  for index, file in ipairs(tab) do
    size = size + 1
  end
  return size
end

function restrict_in(val, a, b)
  lower_bound = math.min(a, b)
  upper_bound = math.max(a, b)
  return math.max(lower_bound, math.min(upper_bound, val))
end

function get_image(fn)
  return Image.load(fn)
end

-- generic table filter function
-- What this does is make a function that makes a copy of the supplied table,
-- where you simply supply a function that returns true
-- if you want the table item to be included, and false if not.
function filtertable (t, f)
  assert (type (t) == "table")
  assert (type (f) == "function")
  local k, v
  local result = {}
  for k, v in ipairs (t) do
    if f (v) then
      table.insert (result, v)
    end -- if
  end -- for
  return result
end -- filtertable

function sorted_ls(dir)
  local raw_files = System.listDirectory(dir)
  files = filtertable(raw_files, function (f_entry) return f_entry.directory == true or string.byte(f_entry.name, 1) ~= string.byte(".", 1) end)
  table.sort(files, function (a, b) return a.name < b.name end)
  return files
end

function get_prev_image_fn(fn)
  cur_dir = System.currentDirectory()
  files = sorted_ls(".")
  last_one = nil
  target_fn = string.lower(fn)
  for index, file in ipairs(files) do
    this_fn = string.lower(file.name)
    if this_fn == target_fn then
      if last_one ~= nil then
        return last_one
      else
        return nil
      end
    end
    if file.directory == false and (string.match(this_fn, "jpg$") ~= nil or string.match(this_fn, "png$") ~= nil) then
      last_one = this_fn
    end
  end
  return nil
end

function get_next_image_fn(fn)
  cur_dir = System.currentDirectory()
  files = sorted_ls(".")
  current_fn = string.lower(fn)
  found = false
  for index, file in ipairs(files) do
    this_fn = string.lower(file.name)
    if found == true and file.directory == false and (string.match(this_fn, "jpg$") ~= nil or string.match(this_fn, "png$") ~= nil) then
      return this_fn
    end
    if this_fn == current_fn then
      found = true
    end
  end
  return nil
end

function view_image(fn)
  local img = get_image(fn)
  local last_btn = Controls.read()
  local off_x = (screen:width() - img:width()) / 2
  local off_y = (screen:height() - img:height()) / 2
  local crop_x = math.max(0, -off_x)
  local crop_y = math.max(0, -off_y)
  local paint_x = math.max(0, off_x)
  local paint_y = math.max(0, off_y)
  
  local show_info = ""
  local show_info_time = nil
  
  while true do
    local btn = Controls.read()
    
    if btn:start() then
      break
    end
    
    local dx = 0
    if btn:left() then
      dx = 10
    elseif btn:right() then
      dx = -10
    end
    if dx ~= 0 then
      if img:width() > 480 then
        off_x = restrict_in(off_x + dx, 480 - img:width(), 0)
      end
      crop_x = math.max(0, -off_x)
      paint_x = math.max(0, off_x)
    end
    
    local dy = 0
    if btn:up() then
      dy = 10
    elseif btn:down() then
      dy = -10
    end
    if dy ~= 0 then
      if img:height() > 272 then
        off_y = restrict_in(off_y + dy, 272 - img:height(), 0)
      end
      crop_y = math.max(0, -off_y)
      paint_y = math.max(0, off_y)
    end
    
    local v_width = math.min(img:width(), screen:width())
    local v_height = math.min(img:height(), screen:height())
    
    screen:clear()
    screen:blit(paint_x, paint_y, img, crop_x, crop_y, v_width, v_height, false)
    if show_info ~=  "" then
      screen:fillRect(0, 130, 480, 12, c_white)
      screen:print((480 - string.len(show_info) * 8) / 2, 132, show_info, c_info)
      if show_info_time + 2 < os.time() then
        show_info = ""
      end
    end
    screen:waitVblankStart()
    screen:flip()
    
    if btn:l() and last_btn ~= btn then
      prev = get_prev_image_fn(fn)
      if prev ~= nil then
        fn = prev
        img = get_image(fn)
        off_x = (screen:width() - img:width()) / 2
        off_y = (screen:height() - img:height()) / 2
        crop_x = math.max(0, -off_x)
        crop_y = math.max(0, -off_y)
        paint_x = math.max(0, off_x)
        paint_y = math.max(0, off_y)
      end
    end
    
    if btn:r() and last_btn ~= btn then
      nxt_img = get_next_image_fn(fn)
      if nxt_img ~= nil then
        fn = nxt_img
        img = get_image(fn)
        off_x = (screen:width() - img:width()) / 2
        off_y = (screen:height() - img:height()) / 2
        crop_x = math.max(0, -off_x)
        crop_y = math.max(0, -off_y)
        paint_x = math.max(0, off_x)
        paint_y = math.max(0, off_y)
      end
    end
    
    if btn:square() and last_btn ~= btn then
      c_info = Color.new(0, 127, 0)
      show_info = "marked as so-so"
      show_info_time = os.time()
      f = io.open(fn..".rank.txt", "w")
      f:write("soso")
      f:close()
      
      -- goes to next image
      nxt_img = get_next_image_fn(fn)
      if nxt_img ~= nil then
        fn = nxt_img
        img = get_image(fn)
        off_x = (screen:width() - img:width()) / 2
        off_y = (screen:height() - img:height()) / 2
        crop_x = math.max(0, -off_x)
        crop_y = math.max(0, -off_y)
        paint_x = math.max(0, off_x)
        paint_y = math.max(0, off_y)
      end
    elseif btn:triangle() and last_btn ~= btn then
      c_info = Color.new(0, 127, 0)
      show_info = "marked as good"
      show_info_time = os.time()
      f = io.open(fn..".rank.txt", "w")
      f:write("good")
      f:close()
      
      -- goes to next image
      nxt_img = get_next_image_fn(fn)
      if nxt_img ~= nil then
        fn = nxt_img
        img = get_image(fn)
        off_x = (screen:width() - img:width()) / 2
        off_y = (screen:height() - img:height()) / 2
        crop_x = math.max(0, -off_x)
        crop_y = math.max(0, -off_y)
        paint_x = math.max(0, off_x)
        paint_y = math.max(0, off_y)
      end
    elseif btn:circle() and last_btn ~= btn then
      c_info = Color.new(0, 0, 255)
      show_info = "marked as excellent"
      show_info_time = os.time()
      f = io.open(fn..".rank.txt", "w")
      f:write("excellent")
      f:close()
      
      -- goes to next image
      nxt_img = get_next_image_fn(fn)
      if nxt_img ~= nil then
        fn = nxt_img
        img = get_image(fn)
        off_x = (screen:width() - img:width()) / 2
        off_y = (screen:height() - img:height()) / 2
        crop_x = math.max(0, -off_x)
        crop_y = math.max(0, -off_y)
        paint_x = math.max(0, off_x)
        paint_y = math.max(0, off_y)
      end
    elseif btn:cross() and last_btn ~= btn then
      c_info = Color.new(255, 0, 0)
      show_info = "delete!"
      show_info_time = os.time()
      f = io.open(fn..".rank.txt", "w")
      f:write("delete")
      f:close()
      
      -- goes to next image
      nxt_img = get_next_image_fn(fn)
      if nxt_img ~= nil then
        fn = nxt_img
        img = get_image(fn)
        off_x = (screen:width() - img:width()) / 2
        off_y = (screen:height() - img:height()) / 2
        crop_x = math.max(0, -off_x)
        crop_y = math.max(0, -off_y)
        paint_x = math.max(0, off_x)
        paint_y = math.max(0, off_y)
      end
    elseif btn:select() and last_btn ~= btn then
      f = io.open(fn..".rank.txt", "r")
      if f == nil then
        c_info = Color.new(255, 0, 0)
        show_info = "not ranked yet!"
        show_info_time = os.time()
      else
        c_info = Color.new(0, 0, 0)
        show_info = "rank: "..f:read()
        show_info_time = os.time()
        f:close()
      end
    end
    last_btn = btn
  end
end

last_btn = Controls.read()

info = "Press 'O' to enter folder or view images"

while true do
  screen:clear()
  if cur_dir ~= last_dir then
    System.currentDirectory(cur_dir)
    cur_dir = System.currentDirectory()
    files = sorted_ls(".")
    folder_name = string.lower(string.sub(last_dir, string.len(cur_dir) + 2))
    if string.len(folder_name) > 0 then
      for index, file in ipairs(files) do
        fname = string.lower(file.name)
        if fname == folder_name then
          cur_idx = index
        end
      end
    else
      cur_idx = 1
      top_idx = 1
    end
    last_dir = cur_dir
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
  if btn:home() then
    return nil
  end
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
      view_image(files[cur_idx].name)
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
