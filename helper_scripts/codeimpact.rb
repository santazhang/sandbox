#!/usr/bin/ruby

# Analyze a git project's history
# shows each commiter's work (# lines changed)
# shows each files' change (lines added/deleted, times changed)

require 'pp'

if ARGV.length > 0
  f = IO.popen("cd '#{ARGV[0]}' && git log --numstat")
else
  f = IO.popen("git log --numstat")
end
lines = f.readlines
f.close

authors = {}
files = {}

author = ""

lines.each do |line|
  case line
  when /^Author: /
    author = line[(line.index ' ')...(line.rindex ' ')].strip
    authors[author] = {} unless authors[author]
    if authors[author][:commit_count]
      authors[author][:commit_count] += 1
    else
      authors[author][:commit_count] = 1
    end
  when /^[0-9]+/
    tab1 =  line.index "\t"
    tab2 = line.index "\t", tab1 + 1
    add_count = line[0...tab1].to_i
    del_count = line[tab1...tab2].strip.to_i
    file = line[tab2..-1].strip
    files[file] = {} unless files[file]
    if files[file][:modify_count]
      files[file][:modify_count] += 1
    else
      files[file][:modify_count] = 1
    end
    if files[file][:add_count]
      files[file][:add_count] += add_count
    else
      files[file][:add_count] = add_count
    end
    if files[file][:del_count]
      files[file][:del_count] += del_count
    else
      files[file][:del_count] = del_count
    end
    if authors[author][:add_count]
      authors[author][:add_count] += add_count
    else
      authors[author][:add_count] = add_count
    end
    if authors[author][:del_count]
      authors[author][:del_count] += del_count
    else
      authors[author][:del_count] = del_count
    end
  end
end

print "Authors:\n"
72.times {print '-'}
print "\n"
authors.each do |name, info|
  print name + "\n"
  print "\tAdded #{info[:add_count]} lines\n"
  print "\tDeleted #{info[:del_count]} lines\n"
  print "\tCommitted #{info[:commit_count]} times\n"
  print "\n"
end

print "Files:\tAdd\tDel\tModify count\n"
72.times {print '-'}
print "\n"
files.each do |name, info|
  print name + "\n"
  print "\t#{info[:add_count]}\t#{info[:del_count]}\t#{info[:modify_count]}\n\n"
end
