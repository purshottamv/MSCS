class UnparsePCC
  def unparse (node)
  	num_threads = 5
	boxer = ShadowBoxing.new do
		rule :Program do |var| var 
			h({:hs => 1}, *var)
		end
		rule :Include do |name|
			v({:is => 0},h({:hs => 0},'#include<',name,'>'),' ')
		end
		rule :Define do |name, *body|
			v({:is => 0},h({:hs => 1},"#define",name,*body.children),' ')
		end
		rule :Struct do |name, *body|
			v({:is => 0},h({:hs => 1},'typedef','struct'),'{',*body[0], h({:hs => 0},'}',name,';'),' ')
		end
		rule :StructDef do |structName, var|
			h({:hs => 1},' ',structName, var, ';')
		end
		rule :Function do |retvals, name, args, body|
			if(args.child(0).to_s().length > 2)
				v({:is => 0},' ',' ',h({:hs => 1}, retvals, name, '(', h({}, *args.children), ')'), body)
			else
				v({:is => 0},' ',' ',h({:hs => 1}, retvals, name, '(', ')'), body, v({:is => 1}))
			end
		end
		rule :OMPPragma do |type1, type2, body|
			# v({:is => 0}, h({:hs => 1}, retvals, name, '(', h({}, *args.children), ')'), body)
		end
		rule :Switch do |args, body|
			v({:is => 1}, h({:hs => 1},' ','switch','(', h({}, *args), ')'), *body)
		end
		rule :Case do |caseid, body|
			v({:is => 1}, h({:hs => 1},' ','case' ,caseid, ':'), body)
		end
		rule :Var do |var| 
			h({:hs => 1},var) 
		end
		rule :Const do |val| val end
        rule :StmtList do |*stmts|
			if(stmts.numChildren > 0)
				v({:is => 1}, *stmts.children)
			end
		end
		rule :DeclarationList do |decls|
			v({:is => 2}, *decls)
		end
		rule :Declaration do |type,args|
			h({:hs => 1},' ',type, h_star({},',',*args),';')
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
		rule :Arrow do |lhs,rhs|
			h({},lhs,'->',rhs)
		end
		rule :ArrayRef do |var,indx|
			if(indx.class == String)
				h({},var,'[',indx,']')				
			elsif(indx.class == Array)
				h({},var,'[',h_star({},'][',*indx.children),']')
			end
		end
		rule :Dot do |lhs,rhs|
			h({},lhs,'.',rhs)
		end
		rule :PointerArg do |param|
			h({},'*',param)
		end
		rule :Block do |body_|
			v({:is => 0},'{', v({:is => 1},*body_),'}',' ')
		end
		rule :ForBlock do |body_|
			v({:is => 0},'{', v({:is => 2},*body_[0]),'}',' ')
		end
		rule :EndScope do |param| 
			v({:is => 0},' ')
		end
		rule :Assignment do |lhs,rhs|
			h({:hs => 1},' ',lhs, '=', rhs, ';')
		end
		rule :IncDec do |lhs,rhs|
			h({:hs => 1},' ',lhs, '=', rhs)
		end
		rule :ElseBlock do |body| 
			v({:is => 0}, 'else', h({:hs => 1}, body))
		end
		rule :IfBlock do |condition,body,elsepart| 
			v({:is => 2}, h({:hs => 1},' ','if (', condition,')'), body, elsepart)
		end
		rule :IfBlock do |condition,body| 
			v({:is => 2}, h({:hs => 1},' ','if (', condition,')'), body)
		end
		rule :For do |init,condition,expression,body| 
			v({:is => 2}, h({:hs => 1},' ','for (', init,condition,';',expression,')'), body)
		end
		rule :While do |condition,body|
			v({:is => 2}, h({:hs => 1}, 'while (', condition,')'), body)
		end
		rule :FunctionCall do |name,val|
			if(name == "omp_set_num_threads")
					# Extract the value
			end
			h({},'  ',name,'(',h_star({:hs=>1},',',*val.children),')',';')
		end
		rule :BreakStmt do || h({}, "break",';')
		end
		rule :OMPSections do
		end
		rule :ContinueStmt do ||  h({}, "continue",';')
		end
		rule :ReturnStmt do |val|   h({:is => 2},'  ',"return ", val,';')
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
			h({:hs => 1}, *val.children)
		end
	  end   # end of ShadowBoxing.new
    box = boxer.unparse_node node
	puts box.to_s
  end  # method unparse ends
end    # Class UnparsePCC ends