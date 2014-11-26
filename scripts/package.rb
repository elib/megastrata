#magic packaging script for the Megastrata binaries and source files.

#written 2009-08-30 by Eli Brody, http://megastructure.org
#inspired by: Gorramit why do I have to do this by hand every time
#(and Wolfgang)

#ruby program to package megastructure files

#uses ruby gem 'rubyzip' ("gem install rubyzip")
#"find" module is core standard

require 'zip/zip'
require 'find'
include Zip

#Megastrata - release
release_dir = "../Megastrata/Megastrata/release/"
license_dir = "../licenses/"
xml_dir 	= "../Megastrata/Megastrata/"
release_filelist = ["Megastrata.exe", "freeglut.dll"]

source_exclude_dir = /\.svn|debug|release|dreams|output|ffmpeg|videos/i
source_exclude_files = /\.dll|\.ncb|\.user|\.sou|buildlog/i
include_anyway = ["../freeglut/freeglut.dll"]

#extract version information
verfile = File.new("../VersionInfo.h", "r")
major = 0
minor = 0

#the cheap way
verfile.each_line do |line|
  majtemp = line[/.*MAJOR_VERSION\s(\d+)/,1]
  mintemp = line[/.*MINOR_VERSION\s(\d+)/,1]
  major = majtemp unless majtemp.nil?
  minor = mintemp unless mintemp.nil?
end

version = "#{major}.#{minor}"

outputdir = "../output/"
if(!File.exist? outputdir) then Dir.mkdir(outputdir) end
release_out = outputdir + "Megastrata_v#{version}.zip"
source_out = outputdir + "MegastrataSource#{version}.zip"

File.delete release_out if(File.exist? release_out)
File.delete source_out if(File.exist? source_out)

zipbase = "megastrata"
puts "creating release executables file"
puts "*********************************"
ZipFile.open(release_out, ZipFile::CREATE) do |zipfile|
  zipfile.mkdir(zipbase)
   
  Dir.new(release_dir).each do |name|
    if release_filelist.member? name
      path = "#{release_dir}#{name}"
      zipfile.add("#{zipbase}/#{name}", path)
      puts "added: #{path}"
    end
  end #release files
  
  Dir.new("../").each do |name|
    if name =~ /.txt/
      path = "../#{name}"
      zipfile.add("#{zipbase}/#{name}", path)
      puts "added: #{path}"
    end
  end #release files
  
  zipfile.mkdir("#{zipbase}/licenses")
  
  Dir.new(license_dir).each do |name|
    if name =~ /\.txt/ then
      path = "#{license_dir}#{name}"
      zipfile.add("#{zipbase}/licenses/#{name}", path)
      puts "added: #{path}"
    end
  end
  
  Dir.new(xml_dir).each do |name|
    if name =~ /\.xml/ then
      path = "#{xml_dir}#{name}"
      zipfile.add("#{zipbase}/#{name}", path)
      puts "added: #{path}"
    end
  end
end

puts "creating source file"
puts "********************"

zipbase = "megastrata-src"
ZipFile.open(source_out, ZipFile::CREATE) do |zipfile|
#  zipfile.mkdir(zipbase)

  Find.find(File.expand_path("../")) do |path|
    if FileTest.directory?(path)
      if ((File.basename(path)[0] == ?. && File.basename(path) != '.') || (path =~ source_exclude_dir ))
        Find.prune       # Don't look any further into this directory.
      else
        puts "added: #{path}"
        next
      end
    else
      if(path !~ source_exclude_files) then
        zipfile.add(File.join(zipbase, path.gsub(File.expand_path("../"), "")), path)
      end
    end
  end
  
  include_anyway.each do |anyway|
    zipfile.add(File.join(zipbase, File.expand_path(anyway).gsub(File.expand_path("../"), "")), anyway)
  end
end
