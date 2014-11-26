#
# $Id: cpplinecount.html,v 1.2 2002/11/19 00:49:37 erngui Exp $
#	http://erngui.com/prog/cpp/cpplinecount.html
# CppLineCount by ErnestoGuisado (erngui@acm.org)
# Count Lines of code in C++ sources.
# Doesn't handle complicated stuff like keywords appearing inside string literals, etc.
#

require 'find'

keywords = %w(case do else enum for if public private protected
  class struct union switch while try catch)

# Humphrey prefers to count '};' too.
punct = /;/

# this regexp catches statements and declarations. 
# The comma catches 'int x,y;' type declarations.
#punct = /(;|,)/

lines = 0
empty = 0
comments = 0
prepro = 0
kwd = 0
statements = 0
files = 0


Find.find(File.expand_path("../Megastrata/Megastrata/")) do |path|
	if FileTest.directory?(path)
	  if ((File.basename(path)[0] == ?. && File.basename(path) != '.'))
		Find.prune       # Don't look any further into this directory.
	  else
		next
	  end
	else
		if(path =~ /.h$|.cpp$/i) then
			files += 1
			f = File.open(path, 'r')
			thelines = f.readlines
			thelines.each do |line|
			  lines += 1
			  if line =~ /^$/
				empty += 1
				next
			  end
			  if line =~ /^\s*\/\//
				comments += 1
				next
			  end
			  if line =~ /^#/
				prepro += 1
				next
			  end
			  line.scan(/\w+/) do |word|
				kwd += 1 if keywords.include?(word)
			  end
			  line.scan(punct) do |p|
				statements += 1
			  end
			end
			f.close
		end
	end
end

puts "Files:               #{files}"
puts "Lines:               #{lines}"
puts "Blank lines:         #{empty}"
puts "Comments(C++ style): #{comments}"
puts "Preprocessor:        #{prepro}"
puts "Keywords:            #{kwd}"
puts "Statements:          #{statements}"
sloc = statements + kwd + prepro # omit blanks and comments
puts "SLOC:                #{sloc}"
