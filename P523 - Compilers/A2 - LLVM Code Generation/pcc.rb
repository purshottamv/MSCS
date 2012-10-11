cwd = File.dirname(__FILE__)
$:.unshift cwd, cwd + '/RubyWrite/lib'

require 'rubywrite'
require 'shadow_boxing'
require 'PCParse/pcparser'

parser = PCParser.new
tt = parser.parse_file

class LLVMAssembly
	include RubyWrite
	
	# Symbol data is the value part of the Symbol Table which is a Hash
	# i.e. SymbolTable = (variableName, SymbolData)
	# SymbolData is an Array [ScopeLevel, datatype, llvmName]
	@@iscopeLevel = 0;		# Index of ScopeLevel in Symbol Data Array
	@@idataType = 1;		# Index of dataType in Symbol Data Array
	@@illvmName = 2;		# Index of llvmName in Symbol Data Array
	
	iconst_string = 0;
	const_string = "str"
	@@iScope = 0;
	@@temp_val = 0					# This is used tosdxx track the temporary variables to be used
	SymbolTableStack = Array.new	# Stack of Symbol Tables depending on Scopes
	@@llvmOutputFile = File.new("llvmexample.ll","w")
	SymbolTableStackTemp = Array.new

	stack1 = []		# Both stack1 and stack2 are used for flattening the expressions
	stack2 = []
	stack3 = []     # This is used to store the datatype of the operands during flattening
	operators = ['+','-','*','/']
	binaryop_datatype = 'i32'

	@@arguments = Array.new			# Temporary list of arguments used to copy function arguments
	@@symbol_table_args = Hash.new	# This is the symbol table for function arguments which are copied later in :Block

	def is_a_number?(s)
	  s.to_s.match(/\A[+-]?\d+?(\.\d+)?\Z/) == nil ? false : true 
	end

	def getSymbolData(var)
		for i in 0...SymbolTableStack.size
			symbol_table = SymbolTableStack[-i - 1]	# Get the latest Symbol Table from the stack
			if(symbol_table.has_key?(var) == true)
				return symbol_table[var]
			end
		end
		return -1
	end

	def resolveVarName(varName)
		symData = getSymbolData(varName)
		if(symData == -1)
			print "Error. Undeclarared Variable ", tempvar
			puts
			exit
		else
			tempVar = symData[@@illvmName]
		end
	end

	def getDataType(varName)
		symData = getSymbolData(varName)
		if(symData == -1)
			print "Error. Undeclarared Variable ", varName
			puts
			exit
		else
			tempVar = symData[@@idataType]
		end
	end

	def insertSymbolTable(varName,datatype)
		symbol_table = SymbolTableStack[SymbolTableStack.size - 1]	# Get the latest Symbol Table from the stack
		symbolData = Array.new(3)
		symbolData[@@iscopeLevel] = @@iScope
		symbolData[@@idataType] = datatype
		newVarName = varName.to_s() + '__' + @@iScope.to_s()
		symbolData[@@illvmName] = newVarName
		symbol_table[varName] = symbolData
		newVarName
	end

	def insertSymbolTable2(varName,datatype,newVar)
		symbolData = Array.new(3)
		symbolData[@@iscopeLevel] = 1
		symbolData[@@idataType] = datatype
		symbolData[@@illvmName] = newVar
		@@symbol_table_args[varName] = symbolData
	end

	def getLLVMType(type)
		llvmType = 'i32'
		if(type == 'char')
		   llvmType = 'i8'
		elsif(type == 'double')
			llvmType = 'double'
		elsif(type == 'float')
			llvmType = 'float'
		end	
		llvmType
	end

	def allocateArguments()
		@@arguments.each do |v|
			@@temp_val = @@temp_val + 1
			varname = @@temp_val.to_s()
			varType = getLLVMType(v[0])
			@@llvmOutputFile.puts "%" + varname + " = alloca " + varType
			@@llvmOutputFile.puts "store " + getLLVMType(v[0]) + " %" + v[1].to_s() + ", " + varType + "* %" + varname
			@@llvmOutputFile.puts
			insertSymbolTable2(v[1],varType,varname)
		end
	end
  define_rw_method :main do |node|
  alltd? node do|n|
      if match? :ConstString[:val], n
		print '@.',const_string,' = private unnamed_addr constant [',lookup(:val).length,' x i8] c\"',lookup(:val),'\00\"'
		puts
		iconst_string = iconst_string + 1
		const_string = const_string + iconst_string.to_s()
      end
  end
  @@llvmOutputFile.puts
  iconst_string = 0;
  const_string = "str"
  parsePidginC node
  @@llvmOutputFile.close
  end

  define_rw_method :parsePidginC do |node|
  alltd? node do |n|
     if match? :Function[:type,:name,:Formals,:Block], n
		 returnType = lookup(:type)
		 @@llvmOutputFile<< "define " + getLLVMType(returnType) + " @" + lookup(:name) + "("
		 if match? :Formals[:_type], lookup(:Formals)
				paramList = lookup(:_type)
				i = 0
				paramList.each do |var|
					@@arguments.push(var)
					if(var[0] == 'int')
						@@llvmOutputFile<< "i32 %" + var[1]
					elsif(var[0] == 'double')
						@@llvmOutputFile<< "double %" + var[1]
					elsif(var[0] == 'char')
						@@llvmOutputFile<< "i8 %" + var[1]
					end
					i = i + 1
					if(i < paramList.count)
						@@llvmOutputFile<< ", "
					end
				end
		 end

		 @@llvmOutputFile.puts ")"
		 @@llvmOutputFile.puts "{"
		 allocateArguments()
		 @@arguments.clear			# After the arguments are allocated to the stack clear it off
	  end

	 if match? :Block[:_val], n
		# Every Block starts a new Scope
		# Build a new Symbol Table and push it in SymbolTableStack
		@@iScope = @@iScope + 1
		 symbol_table = Hash.new			# HashMap for variables and their datatypes
		 if(@@symbol_table_args.size > 0)	# Assign all the symbols from the function arguments into the SymbolTableStack
			@@symbol_table_args.each do |k,v|
				symbol_table[k] = v
			end
			@@symbol_table_args.clear
		 end
		 SymbolTableStack.push(symbol_table)
	 end

	 if match? :EndScope[], n
		@@iScope = @@iScope - 1
		SymbolTableStack.pop		# Pop the Symbol table whose scope has ended
		# At the end of scope
		if(@@iScope == 0)
			@@llvmOutputFile.puts '}'  # will not process anything after a 'return'
			@@llvmOutputFile.puts
		end
	 end

	 if match? :Declaration[:type,:var], n
		 datatype = getLLVMType(lookup(:type))
		 varList = lookup(:var)
		 varList.each do |v|
			if match? :Var[:v], v  # for single declaration
				@@llvmOutputFile<< "%"
				var = getVarName lookup(:v).to_s(), datatype
				@@llvmOutputFile.puts var + " = " + "alloca " + datatype
			 end
		 end	 
     end

     if match? :Assignment[:LHS,:RHS], n
		one? n do |node_val|
			if match? :ConstInt[:val], node_val
				# Check if the variable type too is an integer else convert to variable type
				dataType = getDataType(lookup(:LHS).to_s())
				val = lookup(:val)
				if(dataType == 'double')
					val = Float(val)			
				end
				@@llvmOutputFile.puts "store " + dataType + " " + val.to_s() + ", " + dataType +"* %" + resolveVarName(lookup(:LHS))
			elsif match? :ConstReal[:val], node_val
				# Check if the variable type too is a double else convert to variable type
				dataType = getDataType(lookup(:LHS).to_s())
				val = lookup(:val)
				if(dataType == 'int')
					val = Integer(val)
				end	
				@@llvmOutputFile.puts "store " + dataType + " " + val.to_s() +", " + dataType + "* %" + resolveVarName(lookup(:LHS))
			elsif match? :BinaryOp[:op1,:op,:op2], node_val
				build_binary_op_stack node_val
				build_binary_op_stmt
				# check for type conversion
				dataTypeLHS = getDataType(lookup(:LHS).to_s())
				dataTypeRHS = stack3.pop
				operandRHS = stack2.pop
				if(dataTypeRHS == 'i32' && dataTypeLHS == 'double')
					@@temp_val = @@temp_val + 1
					@@llvmOutputFile.puts "%" + @@temp_val.to_s() + " = " + "sitofp i32 " + operandRHS + " to double"
					operandRHS = "%" + @@temp_val.to_s()
				elsif(dataTypeRHS == 'double' && dataTypeLHS == 'i32')
					@@temp_val = @@temp_val + 1
					@@llvmOutputFile.puts "%" + @@temp_val.to_s() + " = " + "fptosi double " + operandRHS + " to i32"
					operandRHS = "%" + @@temp_val.to_s()
				end
				@@llvmOutputFile.puts "store " + dataTypeLHS + " " + operandRHS + ", " + dataTypeLHS + "* %" + resolveVarName(lookup(:LHS))
			else
				false
			end
	    end	# do ends
		@@llvmOutputFile.puts
     end   # end if match?

      if match? :FunctionCall[:_name,:_args], n
		if(lookup(:_name) == 'printf')
			@@temp_val = @@temp_val + 1
			@@llvmOutputFile<< "%" + @@temp_val.to_s() + " =  call i32 (i8*, ...)* @printf(i8* getelementptr inbounds (["
			one? lookup(:_args) do |node_val|
				if match? :ConstString[:val], node_val
					@@llvmOutputFile<< lookup(:val).length + " x i8]* @." + const_string + " i32 0,i32 0))"
					iconst_string = iconst_string + 1
					const_string = const_string + iconst_string.to_s()
					@@llvmOutputFile.puts
				end
			end	# do ends
		end
      end

      if match? :ReturnStmt[:rval], n
		if match? :ConstInt[:_val], lookup(:rval) 
			@@llvmOutputFile<< "ret i32 " + lookup(:_val)
		elsif match? :ConstString[:_val], lookup(:rval) 
			@@llvmOutputFile<< "ret char " + lookup(:_val)
		elsif match? :ConstReal[:_val], lookup(:rval) 
			@@llvmOutputFile<< "ret double " + lookup(:_val)
		else
			@@temp_val = @@temp_val + 1
			var = resolveVarName(lookup(:rval))
			dataType = getDataType(lookup(:rval).to_s())
			@@llvmOutputFile.puts "%" + @@temp_val.to_s() + " = load " + dataType + "* %" + var
			@@llvmOutputFile<< "ret " + dataType + " %" + @@temp_val.to_s()
		end
		@@llvmOutputFile.puts 
	  end
    end # end alltd?
  end   # end define_rw_method :main

	define_rw_method :getVarName do |varName, datatype|
		# This method 1. Inserts the declared variable in the Current Symbol Table
		# 2. Checks for any re-declaration
		# 3. returns the llvm name by appending __@@iScope value to the variable name

		symbol_table = SymbolTableStack[SymbolTableStack.size - 1]	# Get the latest Symbol Table from the stack
		if(symbol_table.has_key?(varName) == true)
			puts
			print 'Error Variable ',varName,' re-declared'
			puts
			puts "Exiting"
			exit
		else
			newVarName = insertSymbolTable(varName,datatype)
		end
	end

	define_rw_method :checkBinaryOpType do |op|
		op1 = stack2.pop
		op1dtype = stack3.pop
		op2 = stack2.pop
		op2dtype = stack3.pop
		if((op1dtype == op2dtype) && op1dtype == 'i32')
			[op1,op2,'i32']
		elsif((op1dtype == op2dtype) && op1dtype == 'double')
			[op1,op2,'double']
		elsif(op1dtype == 'i32' && op2dtype == 'double')
			@@temp_val = @@temp_val + 1
			@@llvmOutputFile.puts "%" + @@temp_val.to_s() + " = " + "sitofp i32 " + op1 +" to double"
			op1 = "%" + @@temp_val.to_s()
			[op1,op2,'double']
		elsif(op1dtype == 'double' && op2dtype == 'i32')
			@@temp_val = @@temp_val + 1
			@@llvmOutputFile.puts "%" + @@temp_val.to_s() + " = " + "sitofp i32 " + op2 + " to double"
			op2 = "%" + @@temp_val.to_s()
			[op1,op2,'double']
		end
	end

	define_rw_method :build_binary_op_stmt do 
		tempvar = stack1.pop
		if(operators.index(tempvar) != nil)
			largs = checkBinaryOpType(tempvar)
			operand1 = largs[0]
			operand2 = largs[1]
			resultdatatype = largs[2]
			@@temp_val = @@temp_val + 1
			@@llvmOutputFile<< "%" + @@temp_val.to_s() + " = "
			if(tempvar == '+')
				if(resultdatatype == 'double')
					@@llvmOutputFile.puts "fadd " + resultdatatype + " " + operand1 + ", " + operand2
				else
					@@llvmOutputFile.puts "add " + resultdatatype + " " + operand1 + ", " + operand2
				end
			elsif(tempvar == '-')
				if(resultdatatype == 'double')
					@@llvmOutputFile.puts "fsub " + resultdatatype + " " + operand1 + ", " + operand2
				else
					@@llvmOutputFile.puts "sub " + resultdatatype + " " + operand1 + ", " + operand2
				end
			elsif(tempvar == '*')
				if(resultdatatype == 'double')
					@@llvmOutputFile.puts "fmul " + resultdatatype + " " + operand1 + ", " + operand2
				else
					@@llvmOutputFile.puts "mul " + resultdatatype + " " + operand1 + ", " + operand2
				end
			elsif(tempvar == '/')
				if(resultdatatype == 'double')
					@@llvmOutputFile.puts "fdiv " + resultdatatype + " " + operand1 + ", " + operand2
				else
					@@llvmOutputFile.puts "div " + resultdatatype + " " + operand1 + ", " + operand2
				end
			end
			stack2.push("%"+@@temp_val.to_s())
			stack3.push(resultdatatype)
		else
			binaryop_datatype = 'i32'
			if(is_a_number?(tempvar) == false)
				@@temp_val = @@temp_val + 1
				symData = getSymbolData(tempvar)
				if(symData == -1)
					print "Undeclarared Variable ", tempvar
					puts
					exit
				else
					binaryop_datatype = symData[@@idataType]
					var = symData[@@illvmName]
				end
				@@llvmOutputFile.puts "%" + @@temp_val.to_s() + " = load " + binaryop_datatype + "* %" + var
				tempvar = "%" + @@temp_val.to_s()
			else
				# It is constant
				if((tempvar).kind_of? Float)
					binaryop_datatype = 'double'
				end
			end
			stack3.push(binaryop_datatype)
			stack2.push(tempvar)
		end
		if(stack1.empty? == false)
			build_binary_op_stmt
		end
	end  # end of :build_binary_op_stmt

  define_rw_method :build_binary_op_stack do |node|
	 if match? :BinaryOp[:op1, :op, :op2], node
		 stack1.push(lookup(:op))
		 if match? :BinaryOp[:_op1, :_op, :_op2], lookup(:op1)	# Operand 1
			build_binary_op_stack lookup(:op1)
		 elsif match? :ConstInt[:_var], lookup(:op1)
			stack1.push(lookup(:_var))
		 elsif match? :ConstReal[:_var], lookup(:op1)
			stack1.push(lookup(:_var))
		 else
			stack1.push(lookup(:op1))
		 end

		 if match? :BinaryOp[:_op1, :_op, :_op2], lookup(:op2)	# Operand 2
			build_binary_op_stack lookup(:op2)
		 elsif match? :ConstInt[:_var], lookup(:op2)
			stack1.push(lookup(:_var))
		 elsif match? :ConstReal[:_var], lookup(:op2)
			stack1.push(lookup(:_var))
		 else
			stack1.push(lookup(:op2))
		 end
	 end
  end	# :build_binary_op_stack ends
end		# Class ends


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


if(ARGV[0] == "-p")	# Print the parse tree
	puts "Printing the parse tree"
	tt.prettyprint STDOUT	
elsif (ARGV[0] == "-u")
	unparser = UnparsePCC.new
	unparser.unparse tt
elsif (ARGV[0] == "-a")
	puts "Outputting the llvm assembly code in file llvmexample.ll"
    LLVMAssembly.run tt
else
	puts "Help: Use"
	puts "-u : Unparse the prase tree"
	puts "-p : Print the parse tree"
	puts "-a : Print the llvm assembly code"
end
