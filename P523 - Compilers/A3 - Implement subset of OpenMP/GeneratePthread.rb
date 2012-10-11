class GeneratePthread
	include RubyWrite
	
	@@functionCount = 0;		# Keep count of the functions newly created. Also used for creating new name for function
	@@structureCount = 0;		# Keep count of the functions newly created. Also used for creating new name for function
	@@ompParallelBlock = 0		# Check if we are in a parallel block
	@@ompNumOfThreads  = 5
	@@iteratorName = "iter"
	@@ompforiteratorName = "_iter"
	@@limitVarName = "_limit"	# This is the end condition of the Modified omp for loop
	@@ompforiterator = nil
	@@NumThreads = "NUM_THREADS"
	newstructureName = nil
	functionStack = Array.new	# Stack of all the newly generated Functions. this include the corresponding structure
	varlist = []				# Contains list of all the variables declared in the function changed to pointers in structure.
	originalvarlist = []		# Contains list of all the variables declared in the function as it is alongwith its datatype. Used in building assingment statements
	variablelist = []
	structureStack = Array.new	# List of all the newly created structures
	globalDeclarations = Array.new	# List of all newly created global declarations
	defineStack = Array.new
	includeStack = Array.new
	@@OutputBuffer = Array.new

	@@arguments = Array.new			# Temporary list of arguments used to copy function arguments
	@@symbol_table_args = Hash.new	# This is the symbol table for function arguments which are copied later in :Block
	unparser = UnparsePCC.new

	define_rw_method :getFunctionName do		# This function generates a new name for a newly added function
		@@functionCount = @@functionCount + 1
		"function_" + @@functionCount.to_s()
	end

	define_rw_method :getStructureName do		# This function generates a new name for a newly added function
		@@structureCount = @@structureCount + 1
		"structure_" + @@structureCount.to_s()
	end

	define_rw_method :main do |node|
		ret = build :Include["stdio.h"]
		includeStack.push(ret)
		ret = build :Include["pthread.h"]
		includeStack.push(ret)
		ret = build :Include["stdlib.h"]
		includeStack.push(ret)

		alltd? node do |n|		# Do alltd to set the number of Threads
			if match? :FunctionCall[:name,:args], n
				if(lookup(:name) == "omp_set_num_threads")
					lst = lookup(:args)
					@@ompNumOfThreads = lst[0].child(0)
				end
			end
		end # end alltd?

		alltd? node do |n|
			if match? :Function[:type,:name,:Formals,:Block], n
				FunctionBlock(n)
			end
			if match? :Define[:name,:val], n
				defineStack.push(n)
			end
		end # end alltd?
		# Now Create a list of all the functions and embed it in the Program Node i.e. New AST
		functionList = []
		includeStack.each do |x|
			functionList.push(x)
		end
		defineStack.each do |x|
			functionList.push(x)
		end
		structureStack.each do |x|
			functionList.push(x)			# First element in each element of functionStack is a :Function Node. Second element is the list of variables
		end
		globalDeclarations.each do |x|
			functionList.push(x)
		end
		functionStack.each do |x|
			functionList.push(x)			# First element in each element of functionStack is a :Function Node. Second element is the list of variables
		end

		build :Program[functionList]	# Return the new AST
	end   # end define_rw_method :main

	define_rw_method :FunctionBlock do |node|
		newnode = []
		setompthreadsfunction = 0
		threadArrayCreated  = 0
		returnType = node.child(0)
		functionName = node.child(1)
		formalParams = node.child(2)
		alltd? node do |n|
			 if match? :Block[:_val], n
					x = lookup(:_val)
					cnt = 0
					x.each do |ele|
							if(ele.value.to_s() == "OMPParallel")
								ompParallel = ele
								ret = TransformOMPParallel(ompParallel)
								ret.each do |_x|
									newnode.push(_x)
								end
								structnode = build :Declaration[newstructureName,:Var[:ArrayRef["args",@@NumThreads]]]
								newnode.insert(0,structnode)
								somenode = build :Declaration["int",[:Var[@@iteratorName]]]
								newnode.insert(0,somenode)
								somenode = build :Declaration["void",[:Var["*_status"]]]
								newnode.insert(0,somenode)
								if(threadArrayCreated == 0)
									ret = build :Declaration["pthread_t",[:Var[:ArrayRef["threads",@@NumThreads]]]]
									newnode.insert(0,ret)
									threadArrayCreated = 1
									ret = build :Define[@@NumThreads,:ConstInt[@@ompNumOfThreads.to_s()]]
									defineStack.push(ret)
								end
							end
							if(ele.value.to_s() == "OMPParallelFor")

							end
							if(ele.value.to_s() == "OMPSections")

							end
							if(ele.value.to_s() == "Declaration")
								somelist = []
								originalvarlist.push(ele)
								ele.child(1).each do |_x|
									if(_x.child(0).class == RubyWrite::Node && (_x.child(0).value.to_s() == "ArrayRef"))
										variablelist.push(_x.child(0).child(0).to_s())
										somelist.push(build :Pointer[_x.child(0).child(0)])
									else
										variablelist.push(_x.child(0))	# 
										somelist.push(build :Pointer[_x.child(0)])
									end
								end
								varlist.push(build :Declaration[ele.child(0),somelist])
							end
							if(ele.value.to_s() == "FunctionCall" && ele.child(0).to_s() == "omp_set_num_threads")
								setompthreadsfunction = 1	
							end
							if(ele.value.to_s() != "OMPParallel" && ele.value.to_s() != "OMPSections" && setompthreadsfunction != 1)
								newnode.push(ele)
							end
							setompthreadsfunction = 0
							cnt = cnt + 1
					end
			 end
		end
		block = build :Block[newnode]
		func = build :Function[returnType,functionName,formalParams,block]
		functionStack.push(func)
		varlist.clear
		originalvarlist.clear
		newstructureName = nil
		variablelist = nil
	end		# End of function :FunctionBlock

    define_rw_method :TransformOMPParallel do |node|
		pthreadcalls = []
		init = :Assignment[@@iteratorName,:ConstInt["0"]]
		cond = :BinaryOp[@@iteratorName,"<",:ConstString[@@NumThreads]]
		incr = :IncDec[@@iteratorName,:BinaryOp[@@iteratorName,"+",:ConstInt["1"]]]
		newFunctionName = getFunctionName
		newstructureName = getStructureName
		listOfAssignments = buildAssignments
		listOfAssignments.push(:FunctionCall["pthread_create",[:UnaryOp["&",:ArrayRef["threads",@@iteratorName]],"NULL",newFunctionName,:UnaryOp["&",:ArrayRef["args",@@iteratorName]]]])

		forbody = build :Block[listOfAssignments]
		func = createFunction(node,newFunctionName)
		functionStack.push(func)
		varlist.push(build :Declaration["int",[:Var["_threadid"]]])
		structureStack.push(build :Struct[newstructureName, varlist])		# Store the newly created structure
		ret = build :For[init,cond,incr,forbody]
		pthreadcalls.push(ret)

		forbody = build :Block[[:FunctionCall["pthread_join",[:ArrayRef["threads",@@iteratorName],:UnaryOp["&","_status"]]]]]
		ret = build :For[init,cond,incr,forbody]
		pthreadcalls.push(ret)
		pthreadcalls
	end

    define_rw_method :createFunction do |node,newFunctionName|
		# This creates a new function definition
		returnType = "void *"
		functionName = newFunctionName
		param = :Formals[:ConstString["void *args" + @@functionCount.to_s()]]
		node = renameVariables(node)
		functionbody = returnBlock(node)
		build :Function[returnType,functionName,param,functionbody]
	end

    define_rw_method :returnBlock do |node|
		ret = nil
		newstmts = []
		somelist = []
		one? node do |n|
			if match? :Block[:_val], n
				child = n.child(0)
				somelist.push(build :StructDef[newstructureName, "*arg" + @@functionCount.to_s()])
				somelist.push(build :Declaration["int",[:Var["__iter"]]])
				somelist.push(build :Assignment["arg"+@@functionCount.to_s(),:ConstString["(" + newstructureName + " *)args" +@@functionCount.to_s()]])
				cnt = 0
				child.each do |x| 
					if(x.value.to_s() == "FunctionCall")
						y = x.child(1)
						cnt = 0
						y.each do |z|
							if(z.class == RubyWrite::Node && z.value.to_s() == "FunctionCall" && z.child(0).to_s() == "omp_get_thread_num")
								y[cnt] = build :Arrow["arg"+@@functionCount.to_s(),"_threadid"]
								# y[cnt] = :ConstString["arg"+@@functionCount.to_s()+"->_threadid"]
							end
							cnt = cnt + 1
						end
					end
					if(x.value.to_s() == "OMPFor")
						newstmts = transformOMPFor(x.child(0))
						newstmts.each do |y|
							somelist.push(y)
						end
					end
					if(x.value.to_s() == "OMPSections")
						newstmts = transformOMPSections(x.child(0))
						newstmts.each do |y|
							somelist.push(y)
						end
					end
					if(x.value.to_s() == "OMPCritical")
						newstmts = transformOMPCritical(x.child(0))
						newstmts.each do |y|
							somelist.push(y)
						end
					end
					if(x.value.to_s() != "OMPFor" && x.value.to_s() != "OMPSections" && x.value.to_s() != "OMPCritical")
						somelist.push(x)
					end
					cnt = cnt + 1
				end
				ret = build :Block[somelist]
				break
			end
		end
		ret
	end

    define_rw_method :transformOMPFor do |ompforloop|	# We pass the :For[]
		newstmts = []
		newfor = []
		init = nil
		cond = nil
		incdec = ompforloop.child(2)
		totalNumberOfTimes = nil
		@@ompforiterator = ompforloop.child(0).child(0).to_s()
		# Get the limit
		condition = ompforloop.child(1)
		endlimit = condition.child(2)
		if(endlimit.class == RubyWrite::Node && endlimit.value.to_s() == "ConstInt")
			totalNumberOfTimes = endlimit.child(0).to_s()
		end
		if(endlimit.class == String)
			totalNumberOfTimes = endlimit
		end

		ret = build :Declaration["int",[:Var[@@limitVarName],:Var["_temp"],:Var["_mod"]]]
		newstmts.push(ret)
		ret = build :Assignment[@@limitVarName,:BinaryOp[totalNumberOfTimes.to_s(),"/",@@NumThreads]]
		newstmts.push(ret)
		ret = build :Assignment["_temp",:BinaryOp[@@limitVarName,"*",:Arrow["arg"+@@functionCount.to_s(),"_threadid"]]]
		newstmts.push(ret)
		ret = build :Assignment["_mod",:BinaryOp[totalNumberOfTimes.to_s(),"%",@@NumThreads]]
		newstmts.push(ret)

		if(ompforloop.child(0).child(1).value.to_s() == "ConstInt")
			ret = build :ConstString["_temp"]
			init = build :Assignment["__iter",ret]
		end
		if(condition.value.to_s() == "BinaryOp")
			endlimit = build :BinaryOp["_temp","+",@@limitVarName.to_s()]
			cond = :BinaryOp["__iter",ompforloop.child(1).child(1),endlimit]
		end
		if(incdec.child(0).to_s() == @@ompforiterator)
			incdec.children[0] = "__iter"
		end
		if(incdec.child(1).class == RubyWrite::Node && incdec.child(1).value.to_s() == "BinaryOp")
			binaryopnode = incdec.child(1)
			if(binaryopnode.child(0).to_s() == @@ompforiterator)
				binaryopnode.children[0] = "__iter"
			end
			if(binaryopnode.child(2).class == String && binaryopnode.child(2).to_s() == @@ompforiterator)
				binaryopnode.children[0] = "__iter"
			elsif(binaryopnode.child(2).class == RubyWrite::Node && binaryopnode.child(2).value.to_s() == "ConstString")
				binaryopnode.child(2).children[0] = @@ompforiterator;
			end
		end
		@@ompforiterator = nil
		newfor = build :For[init,cond,ompforloop.child(2),ompforloop.child(3)]
		newstmts.push(newfor)
		# This part is introduced if the the number of elements is not exactly the multiple of Number of threads
		stmt1 = build :Assignment["_temp",:BinaryOp[@@limitVarName.to_s(),"*",@@NumThreads]]
		stmt2 = build :Assignment[@@limitVarName,"_mod"]
		innerif = build :IfBlock[:BinaryOp[:Arrow["arg"+@@functionCount.to_s(),"_threadid"],"==",:ConstInt["0"]],:Block[[stmt1,stmt2,newfor]]]
		outerif = build :IfBlock[:BinaryOp["_mod",">",:ConstInt["0"]],:Block[[innerif]]]
		newstmts.push(outerif)
		newstmts
    end

    define_rw_method :transformOMPCritical do |ompCritical|	# We pass the :Block[]
		mutexVariable = "mutex_" + @@functionCount.to_s()
		n = build :FunctionCall["pthread_mutex_lock",[:RefArg[mutexVariable]]]
		ompCritical.child(0).insert(0,n)
		n = build :FunctionCall["pthread_mutex_unlock",[:RefArg[mutexVariable]]]
		ompCritical.child(0).push(n)
		globalDeclarations.push(build :Declaration["pthread_mutex_t",[:Var[mutexVariable]]])
		ompCritical.child(0)
	end

	define_rw_method :transformOMPSections do |ompSections|	# We pass the :Block[]
		newstmts = []
		listOfCases = []
		listOfCaseNumbers = []
		case0 = []
		block = ompSections.child(0)
		caseNumber = 1
		block.each do |stmt|
			if stmt.class == RubyWrite::Node && stmt.value.to_s() == "OMPSection"
				if listOfCaseNumbers.index(caseNumber) == nil
					stmt.child(0).child(0).push(build :BreakStmt[])
					caseNode = build :Case[caseNumber.to_s(),stmt.child(0)]
					listOfCaseNumbers.push(caseNumber)
#					puts "Printing the case node"
#					caseNode.prettyprint STDOUT
#					puts
					listOfCases.push(caseNode)
				else
					caseNode = listOfCases[caseNumber - 1]
					# Get the block list of the case and append the section
					stmt.child(0).child(0).each do |x|
						caseNode.child(1).child(0).insert(-2,x)
					end
				end
				caseNumber = (caseNumber + 1) % @@ompNumOfThreads.to_i
				if(caseNumber == 0)
					caseNumber = caseNumber + 1
				end
			else
				case0.push(stmt)
			end
		end
		case0.push(build :BreakStmt[])
		listOfCases.push(build :Case["0",:Block[case0]])	# Special Handling for case 0
		newstmts.push(build :Switch[:Arrow["arg"+@@functionCount.to_s(),"_threadid"],:Block[listOfCases]])
		newstmts
	end


    define_rw_method :buildAssignments do
		listOfAssignments = []
		originalvarlist.each do |x|		# Each x is :Declaration[type,vars] node
			variables = x.child(1) # this is the list of all :Var[] or :Pointer[] or :ArrayRef[]
			variables.each do |var|
				if(var.class == RubyWrite::Node && (var.value.to_s() == "Var" || var.value.to_s() == "Pointer"))
					if(var.child(0).class == String && var.value.to_s() == "Var")
						listOfAssignments.push(build :Assignment[:Dot[:ArrayRef["args",@@iteratorName],var.child(0)],:RefArg[var.child(0)]])
					elsif(var.child(0).class == String && var.value.to_s() == "Pointer")
						listOfAssignments.push(build :Assignment[:Dot[:ArrayRef["args",@@iteratorName],var.child(0)],:ConstString[var.child(0)]])
					elsif(var.child(0).class == RubyWrite::Node && var.child(0).value.to_s() == "ArrayRef")
						listOfAssignments.push(build :Assignment[:Dot[:ArrayRef["args",@@iteratorName],var.child(0).child(0)],:ConstString[var.child(0).child(0)]])
					end 
				end
			end
		end
		listOfAssignments.push(build :Assignment[:Dot[:ArrayRef["args",@@iteratorName],"_threadid"],@@iteratorName])
		listOfAssignments
	end

    define_rw_method :renameVariables do |_node|
		blocklist = _node.child(0).child(0)
		blocklist.each do |node|
			if node.value.to_s() != "OMPFor"
				_renameVariables(node)
			elsif node.value.to_s() == "OMPFor"
				@@ompforiterator = node.child(0).child(0).child(0).to_s()
				blk = node.child(0).child(3)
				_renameVariables(blk)	
				@@ompforiterator = nil
			end
		end
		_node
    end

	define_rw_method :_renameVariables do |_node|
		alltd? _node do |n|
			if match? :Assignment[:LHS,:RHS], n
				if(lookup(:LHS).class == String && variablelist.index(lookup(:LHS).to_s()) != nil && lookup(:LHS).to_s() != @@ompforiterator)
					n.children[0] = build :PointerArg[:Arrow["arg1",n.child(0).to_s()]]
				end
				if(lookup(:RHS).class == String && variablelist.index(lookup(:RHS).to_s()) != nil && lookup(:RHS).to_s() != @@ompforiterator)
					n.children[1] = build :PointerArg[:Arrow["arg1",n.child(1).to_s()]]
				end
				if(lookup(:RHS).class == RubyWrite::Node && lookup(:RHS).value.to_s() == "FunctionCall" && lookup(:RHS).child(0).to_s() == "omp_get_thread_num")
					n.children[1] = :Arrow["arg"+@@functionCount.to_s(),"_threadid"]
				end
			end   # end if match?
			if match? :BinaryOp[:LHS,:operator,:RHS], n
				if(lookup(:LHS).class == String && variablelist.index(lookup(:LHS).to_s()) != nil && lookup(:LHS).to_s() != @@ompforiterator)
					n.children[0] = build :PointerArg[:Arrow["arg1",n.child(0).to_s()]]
				end
				if(lookup(:RHS).class == String && variablelist.index(lookup(:RHS).to_s()) != nil && lookup(:RHS).to_s() != @@ompforiterator)
					n.children[2] = build :PointerArg[:Arrow["arg1",n.child(2).to_s()]]
				end
			end   # end if match?
			if match? :ConstString[:str], n
#				if(lookup(:str).class == String && variablelist.index(lookup(:str).to_s()) != nil)
#					n.children[0] = "arg"+@@functionCount.to_s()+"->"+n.child(0).to_s()
#				end
			end   # end if match?
			if match? :IncDec[:LHS,:RHS], n
				if(lookup(:LHS).class == String && variablelist.index(lookup(:LHS).to_s()) != nil  && lookup(:LHS).to_s() != @@ompforiterator)
					n.children[0] = build :PointerArg[:Arrow["arg"+@@functionCount.to_s(),n.child(0).to_s()]]
				end
				if(lookup(:RHS).class == RubyWrite::Node && lookup(:RHS).value.to_s() == "BinaryOp" && 
					variablelist.index(lookup(:RHS).child(0).to_s()) != nil  && lookup(:RHS).child(0).to_s() != @@ompforiterator)
						lookup(:RHS).children[0] = build :PointerArg[:Arrow["arg"+@@functionCount.to_s(),lookup(:RHS).child(0).to_s()]]
				end
			end   # end if match?
			if match? :FunctionCall[:name,:args], n
				y = lookup(:args)
				cnt = 0
				y.each do |z|
					if(z.class == String && @@ompforiterator != nil && z ==  @@ompforiterator)
						y[cnt] = "__iter"
					end
					if(z.class == String && @@ompforiterator == nil && variablelist.index(z) != nil)
						y[cnt] = build :PointerArg[:Arrow["arg"+@@functionCount.to_s(),z]]
					end
					if(z.class == RubyWrite::Node && z.value.to_s() == "FunctionCall" && z.child(0).to_s() == "omp_get_thread_num")
						y[cnt] = build :Arrow["arg"+@@functionCount.to_s(),"_threadid"]
						# y[cnt] = :ConstString["arg"+@@functionCount.to_s()+"->_threadid"]
					end
					cnt = cnt + 1
				end
			end
			if match? :ArrayRef[:var,:indx], n
				if(lookup(:var).class == String && variablelist.index(lookup(:var).to_s()) != nil  && lookup(:var).to_s() != @@ompforiterator)
					n.children[0] = build :Arrow["arg"+@@functionCount.to_s(),n.child(0).to_s()]
				end
				y = lookup(:indx)
				cnt = 0
				y.each do |z|	# index is the list of index variables
					if(z.class == String && @@ompforiterator != nil && z == @@ompforiterator)
						y[cnt] = "__iter"
					elsif(z.class == String && variablelist.index(z) != nil)
						y[cnt] = build :PointerArg[:Arrow["arg"+@@functionCount.to_s(),z]]
					end
					cnt = cnt + 1
				end
			end
		end	# end alltd?
	end
end		# Class ends