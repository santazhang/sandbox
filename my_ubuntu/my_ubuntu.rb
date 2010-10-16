#!/usr/bin/ruby

require "fileutils"

if ARGV.length == 0
  puts "Usage: ruby my_ubuntu.rb <server|desktop>"
  exit 1
end

puts "ruby helper script started, running in '#{ARGV[0]}' mode"

def server_mode?
  ARGV[0] == "server"
end

def desktop_mode?
  ARGV[0] == "desktop"
end

def time_now
  now = Time.now
  "#{now.year}#{"%02d" % now.month}#{"%02d" % now.day}#{"%02d" % now.hour}#{"%02d" % now.min}#{"%02d" % now.sec}"
end

# replace "src" to "dest", backup will be performed on "dest"
def do_replace src, dest
  if File.exist? dest
    if (File.directory? src) and (File.directory? dest)
      puts "[replace folder] #{src} --> #{dest}"
      Dir.foreach(src) do |entry|
        if entry == "." or entry == ".."
          next
        end
        sub_src = File.join src, entry
        sub_dest = File.join dest, entry
        do_replace sub_src, sub_dest
      end
    elsif (File.file? src) and (File.file? dest)
      old_dest = "#{dest}.backup.#{time_now}"
      puts "[backup] #{dest} --> #{old_dest}"
      FileUtils.cp dest, old_dest

      puts "[replace file] #{src} --> #{dest}"
      FileUtils.cp src, dest
    end
  else
    dest_dir = File.dirname(dest)
    unless File.exists? dest_dir
      puts "[mkdir] #{dest_dir}"
      FileUtils.mkdir_p dest_dir
    end
    puts "[cp] #{src} #{dest_dir}"
    FileUtils.cp_r src, dest_dir
  end
end

HOME_DIR = ENV["HOME"]
DATA_DIR = File.join File.dirname(__FILE__), "data"

puts "HOME_DIR: #{HOME_DIR}"
puts "DATA_DIR: #{DATA_DIR}"

# config vim
do_replace (File.join DATA_DIR, "vim", "doc"), (File.join HOME_DIR, ".vim", "doc")
do_replace (File.join DATA_DIR, "vim", "nerdtree_plugin"), (File.join HOME_DIR, ".vim", "nerdtree_plugin")
do_replace (File.join DATA_DIR, "vim", "plugin"), (File.join HOME_DIR, ".vim", "plugin")
do_replace (File.join DATA_DIR, "vim", "vimrc"), "/etc/vim/vimrc"

