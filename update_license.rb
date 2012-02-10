#!/usr/bin/env ruby
# encoding: UTF-8

$license =<<EOL
/**
 * Copyright 2010-2012 Riku Palomäki.
 * This file is part of Shaderkit, http://www.shaderkit.org/.
 *
 * Shaderkit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3 as
 * published by the Free Software Foundation.
 *
 * Shaderkit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Shaderkit.  If not, see <http://www.gnu.org/licenses/>.
 */

EOL

$exceptions = [/_lex/, /_yacc/]

def auto
  def glob lst
    len = File.expand_path('x').size-1
    lst.split.inject([]){|s,p| s += Dir[p]}.map{|d| File.expand_path(d)[len..-1]}.uniq
  end
  files = glob '{app,app/*,shaderdb}/*.{cpp,hpp,h} doc/*.cpp std/*.* app/*/*.[yl]'
  ignored = glob File.read('.gitignore').gsub(/^\/?/, '**/')
  (files-ignored).find_all {|f| !$exceptions.any?{|r| r =~ f}}
end

if ARGV.size == 1 && ARGV.first == '-a'
  files = auto
elsif ARGV.empty?
  puts "Usage: #{$0} <files> | -a"
  exit 1
else
  files = ARGV
end

files.each do |filename|
  txt = File.read filename
  old = nil
  if txt.sub!(%r{\A(?:/\*.*?\*/|\s)*}m){|str| old = str; $license } && old != $license
    File.open(filename, 'w') {|f| f.write txt}
    puts "Updated   #{filename}"
  else
    puts "Unchanged #{filename}"
  end
end
