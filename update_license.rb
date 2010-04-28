#!/usr/bin/env ruby

$license =<<EOL
/**
 * Copyright 2010 Riku Palomäki.
 * This file is part of GLSL Lab.
 *
 * GLSL Lab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GLSL Lab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GLSL Lab.  If not, see <http://www.gnu.org/licenses/>.
 */

EOL

if ARGV.empty?
  puts "Usage: #{$0} <files>"
  exit 1
end

ARGV.each do |filename|
  txt = File.read filename
  old = nil
  if txt.sub!(%r{\A(?:/\*.*?\*/|\s)*}m){|str| old = str; $license } && old != $license
    File.open(filename, 'w') {|f| f.write txt}
    puts "Updated   #{filename}"
  else
    puts "Unchanged #{filename}"
  end
end
