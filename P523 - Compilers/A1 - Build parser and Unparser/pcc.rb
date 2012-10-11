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
		rule :Program do |var| var 
			h({:hs => 1}, *var)
		end
		rule :Function do |retvals, name, args, body|
			v({:is => 0}, h({:hs => 1}, retvals, name, '(', h({}, *args.children), ')'), body)
		end
		rule :Var do |var| var end
		rule :Const do |val| val end
        rule :StmtList do |*stmts| v({:is => 1}, *stmts.children) end
		rule :DeclarationList do |*decls|
			v({:is => 2}, *decls)
		end
		rule :FormalParameterList do |*params|
			h({}, *params.children)
		end
		rule :Parameter do |param|
			h({},',',*param.children)
		end
		rule :RefArg do |param|
			h({},'&',param)
		end
		rule :PointerArg do |param|
			h({},'*',param)
		end
		rule :Block do |body| 
			v({},'{', v({},*body),'}')
		end
		rule :Assignment do |lhs,rhs|
			h({:hs => 1}, lhs, '=', rhs, ';')
		end
		rule :ElseBlock do |body| 
			v({:is => 0}, 'else', h({:hs => 1}, body))
		end
		rule :IfBlock do |condition,body,elsepart| 
			v({:is => 2}, h({:hs => 1}, 'if (', condition,')'), body, elsepart)
		end
		rule :For do |init,condition,expression,body| 
			v({:is => 0}, h({:hs => 1}, 'for (', init,condition,';',expression,')'), body)
		end
		rule :While do |condition,body|
			v({:is => 2}, h({:hs => 1}, 'while (', condition,')'), body)
		end
		rule :FunctionCall do |name,val|
			h({},name,'(',h_star({:hs=>1},',',*val.children),')',';')
		end
		rule :Declaration do |type,args|
			h({:hs => 1},type, h_star({},',',*args),';')
		end
		rule :BreakStmt do || h({}, "break",';')
		end
		rule :ContinueStmt do ||  h({}, "continue",';')
		end
		rule :ReturnStmt do ||   h({:is => 2}, "return",';')
		end
		rule :ReturnStmt do |val|   h({:is => 2}, "return ", val,';')
		end
		rule :ConstInt do |val| h({}, val)
		end
		rule :ConstString do |val| h({}, val)
		end
		rule :ConstReal do |val| 
			h({}, val)
		end
		rule :BinaryOp do |lhs,operator,rhs| 
			h({}, lhs,operator,rhs)
		end
		rule :UnaryOp do |operator,val| 
			h({}, operator,val)
		end
		rule :Pointer do |val| 
			h({},'*' ,val)
		end
		rule :FunctionDecl do |name,args|
			h({:hs => 0}, name, '(', *args.children[0], ')')
		end
		rule :Formals do |*val|
			puts ":Formals"
			puts val
			h({:hs => 1}, *val.children)
		end
	  end   # end of ShadowBoxing.new
    box = boxer.unparse_node node
    puts box.to_s
  end  # method unparse ends
end    # Class UnparsePCC ends

unparser = UnparsePCC.new
unparser.unparse tt
