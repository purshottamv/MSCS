require 'rubywrite'

# We assume that the Flattener is invoked AFTER all unary and binary operations have been renamed to their
# function call equivalents.  As a result, there is no need to handle unary and binary operations here.
class Flattener
  include RubyWrite

  def initialize
    @tmp_counter = 0
    @subs_stack = []  # keep track of the variables whose subscripts we might be processing
  end

  def new_tmp_name
    @tmp_counter += 1
    "PARAM_tmp#{@tmp_counter}"
  end

  def last_tmp_name
    "PARAM_tmp#{@tmp_counter}"
  end

  def flatten_stmt_array a
    a.inject([]) {|s, x| s.concat flatten_stmt(x)}
  end


  # flatten_ifs returns the recursively flattened if-statement.  Elseifs are converted to nested ELSE-IFs.
  define_rw_method :flatten_ifs do |i_list|
    s = []
    r = nil
    i_list.each do |b|
      if match? :If[:cond,:StmtList[:stmts]], b
        f_cond_stmts,cond_var = flatten_expr_completely(lookup(:cond))
        s.concat f_cond_stmts
        e = []  # create a placeholder for ELSE statements
        ie = :IfElse[cond_var,:StmtList[flatten_stmt_array(lookup(:stmts))],:StmtList[e]]
        s << (r ? :Silent[ie] : ie) # Hack to prevent the top-level from getting :Silent
        r ||= s
        s = e
      elsif match? :Else[:StmtList[:stmts]], b
        s.replace flatten_stmt_array(lookup(:stmts))
      else
        raise Fail.new("Expecting an IF or ELSE, got node: #{b.to_string}")
      end
    end
    r
  end

  # flatten_switch returns the recursively flattened switch-statement.  Cases are converted to nested ELSE-IFs.
  define_rw_method :flatten_switch do |expr, c_list|
    s,e_var = flatten_expr_completely(expr)
    r = nil
    c_list.each do |c|
      if match? :Case[:cond, :StmtList[:stmts]], c
        f_cond_stmts,cond_var = flatten_expr_completely(lookup(:cond))
        s.concat f_cond_stmts
        e = [] # create a placeholder for ELSE statements
        s << :Silent[:Assignment[:Var[new_tmp_name],'=',:BinaryInfixOp[cond_var,'==',e_var]]]
        ie = :IfElse[last_tmp_name,:StmtList[flatten_stmt_array(lookup(:stmts))],:StmtList[e]]
        s << (r ? :Silent[ie] : ie) # Hack to prevent the top-level from getting :Silent
        r ||= s
        s = e
      elsif match? :Default[:StmtList[:stmts]], c
        s.replace flatten_stmt_array(lookup(:stmts))
      else
        raise Fail.new("Expecting a CASE, got node: #{c.to_string}")
      end
    end
    r
  end
  # Statement flattener always returns an array of flattened statements.
  define_rw_rewriter :flatten_stmt do
    rewrite :Silent[:stmt] do |n|
      f_stmts = flatten_stmt(lookup(:stmt))
      if f_stmts.length > 0
	f_stmts[f_stmts.length-1] = :Silent[f_stmts.last] 
	f_stmts[f_stmts.length-1].attributes = n.attributes.clone
      end
      f_stmts
    end

    rewrite :For[:var,:range,:StmtList[:stmts]] do
      f_range_stmts,range_var = flatten_expr_completely(lookup(:range))
      f_range_stmts << :Silent[:Assignment[lookup(:var),'=',:List[[:Matrix[[]]]]]]
      loop_var = new_tmp_name
      loop_body = flatten_stmt_array(lookup(:stmts))
      loop_body.insert(0, :Silent[:Assignment[lookup(:var),'=',:Var[loop_var]]])
      f_range_stmts << :For[:Var[loop_var],range_var,:StmtList[loop_body]]
    end

    rewrite :While[:cond,:StmtList[:stmts]] do
      f_cond_stmts,cond_var = flatten_expr_completely(lookup(:cond))
      f_cond_stmts << :While[cond_var,:StmtList[flatten_stmt_array(lookup(:stmts))+f_cond_stmts.dclone]]
    end

    rewrite :Ifs[:if_list] do
      flatten_ifs lookup(:if_list)
    end

    rewrite :Switch[:expr, :case_list] do |n|
      flatten_switch lookup(:expr), lookup(:case_list)
    end

    rewrite :Break[] do |n|
      [n]
    end

    rewrite :Continue[] do |n|
      [n]
    end

    rewrite :Return[] do |n|
      [n]
    end
    # Assume that everything else is an expression, being treated as a statement
    default do |n|
      f_stmts,f_expr = flatten_expr(n)
      f_stmts << f_expr
    end
  end


  # flatten_expr returns a pair (Array of size 2), [stmts, flat_expr], where "stmts" is a (possibly empty) array of
  # statements that compute intermediate values and "flat_expr" is the resulting flattened expression.  The order of
  # exressions here is the same as in Grammar.txt.
  define_rw_rewriter :flatten_expr do
    rewrite :Subscript[:Var[:varname],:expr_list] do
      f_stmts = []
      @subs_stack.push lookup(:varname)
      f_exprs = lookup(:expr_list).map {|e| f_s,f_e = flatten_expr_completely(e); f_stmts += f_s; f_e}
      @subs_stack.pop
      [f_stmts, :Subscript[:Var[lookup(:varname)], f_exprs]]
    end

    rewrite :Range[:l, :s, :u] do
      l_stmts,l_var = flatten_expr_completely(lookup(:l))
      s_stmts,s_var = flatten_expr_completely(lookup(:s))
      u_stmts,u_var = flatten_expr_completely(lookup(:u))
      [l_stmts+s_stmts+u_stmts, :Range[l_var,s_var,u_var]]
    end

    rewrite :FunctionCall[:name, :Args[:args]] do
      a_stmts,a_vars = lookup(:args).inject([[],[]]) do |arg_list, arg|
        f_a,f_v = flatten_expr_completely(arg)
        arg_list[0] += f_a
        arg_list[1] << f_v
        arg_list
      end
      [a_stmts, :FunctionCall[lookup(:name), :Args[a_vars]]]
    end

    rewrite :Assignment[:lhs,'=',:rhs] do
      f_stmts_rhs,f_rhs = flatten_expr(lookup(:rhs))
      f_stmts_lhs,f_lhs = flatten_expr(lookup(:lhs))
      [f_stmts_rhs+f_stmts_lhs, :Assignment[f_lhs,'=',f_rhs]]
    end

    rewrite :MultiAssignment[:lhs_list,'=',:rhs] do
      f_stmts_rhs,f_rhs = flatten_expr(lookup(:rhs))
      f_stmts_lhs,f_lhs_list = lookup(:lhs_list).inject([[],[]]) do |f_list,lhs|
        f_l,f_v = flatten_expr(lhs)
        f_list[0] += f_l
        f_list[1] << f_v
        f_list
      end
      [f_stmts_rhs+f_stmts_lhs, :MultiAssignment[f_lhs_list,'=',f_rhs]]
    end

    # :CellIndex is handled similarly to :Subscript
    rewrite :CellIndex[:var,:expr_list] do
      f_stmts = []
      f_exprs = lookup(:expr_list).map {|e| f_s,f_e = flatten_expr_completely(e); f_stmts += f_s; f_e}
      [f_stmts, :CellIndex[lookup(:var), f_exprs]]
    end

    # :Matrix has an array (columns) of arrays (column vectors)
    rewrite :Matrix[:v_list] do
      f_stmts = []
      vecs = lookup(:v_list).map {|v| v.map {|e| f_s,f_e = flatten_expr_completely(e); f_stmts += f_s; f_e}}
      [f_stmts, :Matrix[vecs]]
    end
    # :Cell has an array (columns) of arrays (column vectors) (similar to :Matrix)
    rewrite :Cell[:v_list] do
      f_stmts = []
      vecs = lookup(:v_list).map {|v| v.map {|e| f_s,f_e = flatten_expr_completely(e); f_stmts += f_s; f_e}}
      [f_stmts, :Cell[vecs]]
    end
    # :FieldVar has an expression and a field name (which is raw string)
    rewrite :FieldVar[:expr,:field] do
      f_stmts,f_expr = flatten_expr_completely(lookup(:expr))
      [f_stmts, :FieldVar[f_expr,lookup(:field)]]
    end

    # The default cases catches :Var, :Const, and :List (and may be others that we might have missed).  :List is an
    # Octave idiosyncracy that wraps any constant value
    default do |n|
      [[], n]
    end
  end


  # In addition to flattening the given expression, flatten_expr_completely creates a new variable to store the value.
  # It also inserts a statement to assign the computed value to this new variable.  It returns a pair (array of two
  # elements), [stmts, var], where "stmts" is the (possibley empty) array of statements that compute intermediate
  # values and "var" is the new variable name enclosed in :Var.
  define_rw_rewriter :flatten_expr_completely do
    # We have to write special cases for :List, :Var, and :Const here again, because flatten_expr_completely may also be
    # called directly, not necessarily through flatten_expr.
    rewrite :List[:_] do |n|
      [[], n]
    end

    rewrite :Var[:name] do |n|
      if ((v=lookup(:name)) == '__end__')
        [[:Silent[:Assignment[:Var[new_tmp_name],
                              '=',
                              :FunctionCall['PARAM_get_num_elements',
                                            :Args[[:Var['SXF_size_'+@subs_stack[-1]]
                                                  ]]
                                           ]
                             ]
                 ]],
         :Var[last_tmp_name]
        ]
      else
        [[], n]
      end
    end

    rewrite :Const[:_] do |n|
      [[], n]
    end

    default do |n|
      flat_stmts,flat_expr = flatten_expr(n)
      # Irrespective of whether or not the original expression was silent, the new assignment statements must be silent.
      [flat_stmts << :Silent[:Assignment[:Var[new_tmp_name],'=',flat_expr]], :Var[last_tmp_name]]
    end
  end


  def main (node)
    if match? :Function[:retvals, :name, :args, :StmtList[:stmts]], node
      # we got a full function
      build :Function[:retvals, :name, :args, :StmtList[flatten_stmt_array(lookup(:stmts))]]
    elsif match? :StmtList[:stmts], node
      # we got a script
      :StmtList[flatten_stmt_array(lookup(:stmts))]
    else
      raise Fail.new("Expected a :Function, got node #{node.to_string}")
    end
  end
end
