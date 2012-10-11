require 'rake/rdoctask'
require 'rake/clean'

$PARSE_DIR = "PCParse"

def inside_parse_dir ary
  ary.map {|f| "#{$PARSE_DIR}/#{f}"}
end

task :default

task :all => :default

desc "Compile the scanner C extension and the PidginC parser"
task :default => [:startup] + inside_parse_dir(["lex.yy.c", "Makefile", "pcparser.rb"]) + [:make, :finishup]

CLEAN.include(inside_parse_dir ["*.o","*.output"])
CLOBBER.include(inside_parse_dir ["Makefile","depend","lex.yy.c","pcparser.rb","*.bundle","*.so"])

task :startup do
  puts "COMPILING ..."
end

task :finishup do
  puts "\nDONE"
end

file "#{$PARSE_DIR}/lex.yy.c" => ["#{$PARSE_DIR}/lex.l"] do
  Dir.chdir "#{$PARSE_DIR}" do
    puts "\nGENERATING THE SCANNER"
    sh "flex lex.l"
  end
end

file "#{$PARSE_DIR}/Makefile" => ["#{$PARSE_DIR}/extconf.rb"] do
  Dir.chdir "#{$PARSE_DIR}" do
    puts "\nGENERATING Makefile FROM extconf.rb"
    sh "ruby extconf.rb"
  end
end

file "#{$PARSE_DIR}/pcparser.rb" => ["#{$PARSE_DIR}/pcparser.y"] do |t|
  Dir.chdir "#{$PARSE_DIR}" do
    puts "\nGENERATING THE PARSER"
    sh "racc -v pcparser.y -o pcparser.rb"
  end
end

task :make do
  Dir.chdir "#{$PARSE_DIR}" do
    puts "\nINVOKING make"
    sh "make"
  end
end

Rake::RDocTask.new do |rd|
  rd.main = "README"
  rd.rdoc_dir = 'doc/rdoc'
  rd.title = 'Support code for P523'
  rd.rdoc_files.include("README", inside_parse_dir(["extconf.rb", "lex.l", "pcparser.y", "tokens.h", "scanner.c"]), 
                        "*.rb", "Rakefile")
  rd.options = ['--inline-source']
end
