if ARGV.size != 1
  $stderr.puts "Usage: #{$0} <Build dir root>"
  exit 1
end

iscc = "C:\\Program Files (x86)\\Inno Setup 5\\ISCC.exe"
root = File.expand_path ARGV.first

Dir.chdir(root) do
  system "nmake install \"INSTALL_ROOT=#{root.gsub('/', '\\')}\\install\""
end
exit $?.exitstatus unless $?.success?

version = File.read("#{root}/version.hpp").scan(/^#define SHADERKIT_VERSION "(.*)"/).to_s
release = File.read("#{root}/app/Makefile").scan(/^first: (release|debug)/)[0][0] == "release"
project = File.read("#{root}/Makefile").scan(/^# Project:\s+(.+)$/)[0][0]
qmake = File.read("#{root}/Makefile").scan(/^QMAKE\s+=\s+(.+)$/)[0][0]

qt = File.expand_path "#{qmake}/../.."
x64 = qt.include?('x64')

src = File.expand_path "#{project}/.."

File.open("#{root}/install/local.iss", 'w') do |local|
  local.puts "#define ShaderkitVersion \"#{version}\""
  local.puts "#define ShaderkitRoot \"#{root}/install\""
  local.puts "#define QtRoot \"#{qt}\""
  local.puts "#define QtSuffix \"#{'d' unless release}4\""
  local.puts "#define ShaderkitArchUI \"#{x64 ? '64' : '32'}-bit\""
end

local = File.expand_path "#{root}/install"

Dir["#{root}/install/setup/*.exe"].each{|f| File.unlink f}
system "\"#{iscc}\" /q \"/o#{root}/install/setup\" \"/i#{local}\" \"#{src}/scripts/win/installer/Shaderkit.iss\""
exit $?.exitstatus unless $?.success?

Dir["#{root}/install/setup/*.exe"].each{|f| puts f}
