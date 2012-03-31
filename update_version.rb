#!/usr/bin/ruby

input_file = ARGV.shift

dir = File.dirname(input_file)
hash = ''
ref = ''
begin
  ref = File.read("#{dir}/.git/HEAD").scan(/^ref: (.+)$/)[0][0].strip
  ref = "#{dir}/.git/"+ref
  full_hash = File.read(ref)
  hash = full_hash[0..12]
rescue
end

ref = File.expand_path ref if File.exists?(ref)

if ARGV.first == '--dep'
  puts ref
	head = "#{dir}/.git/HEAD"
	head = File.expand_path head if File.exists?(head)
	puts head
  exit 0
end

output_file = ARGV.shift
version = File.read(input_file).strip
lst = version.split('.', 3)
lst[2], lst[3] = lst[2].scan(/^(\d+)(.*)/)[0]

File.open(output_file, 'w') do |f|
  f.puts "#ifndef VERSION_HPP"
  f.puts "#define VERSION_HPP"
  f.puts
  f.puts "// This file is also included from app.rc, that doesn't know how to parse namespaces"
  f.puts "#ifdef __cplusplus"
  f.puts "namespace ShaderKit {"
  f.puts "  static const int MAJOR = #{lst[0]};"
  f.puts "  static const int MINOR = #{lst[1]};"
  f.puts "  static const int REVISION = #{lst[2]};"
  f.puts "  static const char* EXTRA = \"#{lst[3]}\";"
  f.puts "  static const char* HASH = \"#{hash}\";"
  f.puts "  static const char* STR = \"#{version}\";"
  f.puts "  static const char* STR_HASH = \"#{hash.empty? ? version : version+'+'+hash}\";"
  f.puts "}"
  f.puts "#else"
  f.puts "#define SHADERKIT_VERSION \"#{version}\""
  f.puts "#endif"
  f.puts ""
  f.puts "#endif"
end
