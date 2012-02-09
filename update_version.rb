#!/usr/bin/ruby

dir = File.dirname(__FILE__)
hash = ''
ref = ''
begin
  ref = File.read("#{dir}/.git/HEAD").scan(/^ref: (.+)$/)[0][0].strip
  ref = "#{dir}/.git/"+ref
  full_hash = File.read(ref)
  hash = full_hash[0..12]
rescue
end

if ARGV.first == '--dep'
  puts ref
  puts "#{dir}/.git/HEAD"
  exit 0
end

version = File.read("#{dir}/version").strip
lst = version.split('.', 3)
lst[2], lst[3] = lst[2].scan(/^(\d+)(.*)/)[0]

File.open("#{dir}/version.hpp", 'w') do |f|
  f.puts "#ifndef VERSION_HPP"
  f.puts "#define VERSION_HPP"
  f.puts
  f.puts "namespace ShaderKit {"
  f.puts "  static const int MAJOR = #{lst[0]};"
  f.puts "  static const int MINOR = #{lst[1]};"
  f.puts "  static const int REVISION = #{lst[2]};"
  f.puts "  static const char* EXTRA = \"#{lst[3]}\";"
  f.puts "  static const char* HASH = \"#{hash}\";"
  f.puts "  static const char* STR = \"#{version}\";"
  f.puts "  static const char* STR_HASH = \"#{hash.empty? ? version : version+'+'+hash}\";"
  f.puts "}"
  f.puts ""
  f.puts "#endif"
end
