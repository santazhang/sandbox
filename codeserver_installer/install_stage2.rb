#!/usr/bin/ruby

puts "Required packages installed, entering install stage 2..."

# prompt for install dir
default_install_dir = "#{ENV['HOME']}/codeserver"
puts "Where should the code server be installed? [#{default_install_dir}]"
install_dir = gets.chomp
if install_dir.include? " " or install_dir.include? "\t"
  puts "Error: whitespace not allowed in path!"
  exit 1
end
install_dir = default_install_dir if install_dir == ""
puts "Installing to #{install_dir}..."

# untar redmine & gitosisi, make svn-repo dir
system "
  mkdir -p #{install_dir}
  cp #{File.dirname __FILE__}/README #{install_dir}
  cp #{File.dirname __FILE__}/data/redmine-0.9.3.tar.gz #{install_dir}
  cp #{File.dirname __FILE__}/data/gitosis.tar.gz #{install_dir}
  cd #{install_dir}
  tar xzf redmine-0.9.3.tar.gz
  tar xzf gitosis.tar.gz
  rm redmine-0.9.3.tar.gz
  rm gitosis.tar.gz
  mkdir -p svn-repo
  mkdir -p git-repo
  mkdir -p backup
"

# config redmine
system "
  cp #{File.dirname __FILE__}/data/database.yml #{install_dir}/redmine-0.9.3/config
  cp #{File.dirname __FILE__}/data/Rakefile #{install_dir}
  cp #{File.dirname __FILE__}/data/backup.ini.example #{install_dir}
  cd #{install_dir}/redmine-0.9.3
  rake generate_session_store
  rake db:migrate RAILS_ENV=production
  rake redmine:load_default_data RAILS_ENV=production
"

# config gitosis
system "
  cd #{install_dir}/gitosis
  sudo python setup.py install > /dev/null
  cd ..
  sudo rm -rf gitosis
"
# check if has key file
unless File.exists? "#{ENV['HOME']}/.ssh/id_rsa.pub"
  system "yes | ssh-keygen -t rsa -N \"\" -f #{ENV["HOME"]}/.ssh/id_rsa"
end

system "
  cp #{ENV["HOME"]}/.ssh/id_rsa.pub /tmp/id_rsa.pub
  cd #{install_dir}
  sudo -H -u git gitosis-init < /tmp/id_rsa.pub
  rm /tmp/id_rsa.pub
  sudo chmod 755 /home/git/repositories/gitosis-admin.git/hooks/post-update
  git clone git@localhost:gitosis-admin.git
"

puts "Everything done!"
puts "NOTE: default Redmine admin account:"
puts "  * username: admin"
puts "  * password: admin"
puts "See README in #{install_dir} for instructions."

