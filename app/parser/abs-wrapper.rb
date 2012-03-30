ARGV.map! do |arg|
  File.exists?(arg) ? File.expand_path(arg) : arg
end
exec *ARGV
