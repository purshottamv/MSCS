cwd = File.dirname(__FILE__)
$:.unshift cwd, cwd + '/RubyWrite/lib'

require 'PCParse/pcparser'
require 'shadow_boxing'

# We need to first create a parser object
parser = PCParser.new

# The parser can also parse from a file (stdin, actually).
# puts parser.parse_file
tt = parser.parse_file
if(ARGV[0]=="-p")
	tt.prettyprint STDOUT
	puts
end
# prettyprint doesn't print a newline at the end; so, call puts once more
puts

class UnparsePCC
  def unparse (node)
    boxer = ShadowBoxing.new do
		rule :Program do |var| var end
		rule :Function do |retvals, name, args, body|
			v({:is => 2},
		    h({},
			"function ",
			"[", h_star({}, ", ", *retvals.children), "] = ",
			name,
			"(", h_star({}, ", ", *args.children), ")"),
		  body)
		end
		rule :Var do |var| var end
		rule :Const do |val| val end
                rule :StmtList do |stmts| v({}, *stmts.children) end
	  end   # end of ShadowBoxing.new
    box = boxer.unparse_node node
    puts box.to_s
  end  # method unparse ends
end    # Class UnparseMATLAB ends

unparser = UnparsePCC.new
unparser.unparse tt
