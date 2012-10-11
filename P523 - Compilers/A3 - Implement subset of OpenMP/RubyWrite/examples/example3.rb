require 'rubywrite'

#
# The following is a very simple, but hopefully expressive enough to be
# vaguely useful, little language to use for testing purposes with the
# RubyWrite framework.  Eventually maybe I'll write a TreeTop grammar or
# something like that for the syntax, but right now, I'll just expect
# things to come in through the :Node[...] interface.  Also, perhaps
# at some point we can grow this example language, to demonstrate more
# features as we continue to work on this, it would be nice to show how
# to do some functional programming and OOP stuff, which is not really
# demonstrated in the Stratego/XT tutorial stuff.
#
# Id       --> [a-zA-Z_][a-zA-Z0-9_]*               - identifiers
# FunDef   --> fun Id (Id*) { Expr* }               - function def
# Expr     --> Id                                   - variable ref
#           |  Id = Expr                            - assignment
#           |  Id[Expr] = Expr                      - array assign
#           |  if Expr then Expr* (else Expr*)? end - if expr
#           |  while Expr do Expr* end              - while expr
#           |  not Expr                             - boolean not
#           |  Expr or Expr                         - boolean or
#           |  Expr and Expr                        - boolean and
#           |  Expr + Expr                          - addition
#           |  Expr - Expr                          - subtraction 
#           |  Expr * Expr                          - multiplication
#           |  Expr / Expr                          - division
#           |  Expr % Expr                          - modulo
#           |  Expr ^ Expr                          - exponent
#           |  Expr == Expr                         - equal
#           |  Expr < Expr                          - less than
#           |  Expr > Expr                          - greater than
#           |  Expr <= Expr                         - less than or equal
#           |  Expr >= Expr                         - greater than or equal
#           |  '(' Expr ')'                         - grouping
#           |  defined? Expr                        - boolean to indicate if
#                                                     something is currently
#                                                     defined (for array)
# Literals --> Number                               - all numbers
#           |  Boolean                              - booleans
#           |  Array                                - array
# Number   --> [0-9]*(.[0-9]*)?                     - numbers (int/float)
# Boolean  --> true                                 - bool true 
#           |  false                                - bool false
# Array    --> [ Expr* ]                            - array literal
#             
# This will be expressed in the following way in nodes:
# Id       --> "identifer"                        - identiiers (by ctxt)
# FunDef   --> :FunDef["name",:Params["p1", ...], [expr, ...]]
# Expr     --> :Var["idenifier"]
#           |  :ArrayRef[var_expr,idx_expr]
#           |  :Assign[:Var["identifier"],expr]
#           |  :Assign[:ArrayRef[var_expr,idx_expr],expr]
#           |  :If[expr,[expr, ...]]                  - if w/o else
#           |  :If[expr,[expr, ...],[expr, ...]] - if w/else
#           |  :While[expr,[expr, ...]]
#           |  :Not[expr]
#           |  :Or[expr,expr]
#           |  :And[expr,expr]
#           |  :Add[expr,expr]
#           |  :Sub[expr,expr]
#           |  :Mul[expr,expr]
#           |  :Div[expr,expr]
#           |  :Mod[expr,expr]
#           |  :Exp[expr,expr]
#           |  :Equal[expr,expr]
#           |  :GreaterThan[expr,expr]
#           |  :LessThan[expr,expr]
#           |  :GreaterEqual[expr,expr]
#           |  :LessEqual[expr,expr]
#           |  :Defined[expr]          - defined?
# Literals --> :Lit["number"]          - literal number
#           |  :True                   - literal true
#           |  :False                  - literal false
#           |  :Array[expr, expr, ...] - lit array (contents may not be lit)
# Internal --> :Nil                    - nil not express
#
# Semantics are the expected semantics (dynamic-language like) but to clarify:
#    * Anything not false is true
#    * Arrays are extended automagically by assinging to a reference past the
#      end of the array
#    * Array references past the end of an array result in internal Nil which
#      can be checked by using defined?
#


# The following transformer gets rid of one armed ifs in preperation for the 
# simplify not step.
#
class SimplifyIf < RubyWrite::ReWriter
  include RubyWrite::Traversals

  def main
    topdown { remove_one_armed_if }
    id
  end

  def remove_one_armed_if
    if succeeds? { match :If[:test,:body] }
      puts "found one-armed if"
      build! :If[:test,:body,:Nil[]]
    else
      puts "returning id"
      build! id
    end
  end
end

r1 = SimplifyIf.run(:If[:Not[:GreaterThan[:Lit["1"],:Lit["1"]]], [:Lit["5"]]])
r2 = SimplifyIf.run(:FunDef["foo",:Params["x"],
                            [:If[:Not[:LessThan[:Var["x"],:Lit["0"]]],
                                [:Assign[:Var["x"],
                                    :Sub[:Lit["0"],:Var["x"]]]]],
                              :Var["x"]]])

puts "r1: #{r1.to_string}"
puts "r2: #{r2.to_string}"

__END__

# Another simple transformer that converts:
#   if not(x) then c else a end into:
#   if x then a else c end
class SimplifyNot < RubyWrite::ReWriter
  include RubyWrite::Traversals

  def main
    topdown { remove_unecessary_not }
    id
  end

  def remove_unecessary_not
    if succeeds? { match :If[:Not[:expr],:conseq,:alt] }
      build! :If[:expr,:alt,:conseq]
    else
      id
    end
  end
end

r1 = SimplifyNot.run(r1)
r2 = SimplifyNot.run(r2)

puts "r1: #{r1.to_string}"
puts "r2: #{r2.to_string}"


# Helper method for the constant propagation stuff:
unless String.respond_to? :to_num
  class String
    def to_num
      if self.include? '.'
        self.to_f
      else
        self.to_i
      end
    end
  end
end

unless Hash.respond_to? :meet
  class Hash
    def meet other
      kill_list = []
      self.keys.each do |k|
        unless other.has_key? k and self[k] == other[k]
        end
      end
    end
  end
end

class ConstProp < RubyWrite::ReWriter
  include RubyWrite::Traversals
  include RubyWrite::PrettyPrint

  @prop_env = [{}]

  def main
  end

  def constant_prop
    if succeeds? { match :FunDef[:name,:params,:body] }
      @prop_env.push {}
      new_body = call_in_context(id { :body }) { topdown { constant_prop } }
      @prop_env.pop
      build! :FunDef[:name,:params,new_body]
    elsif succeeds? { match :If[:test,:conseq,:alt] }
      new_test = call_in_context(id { :test }) { topdown { constant_prop } }
      @prop_env << @prop_env[-1].clone
      new_conseq = call_in_context(id { :conseq }) { topdown { constant_prop } }
      @prop_env << @prop_env[-2].clone
      new_alt = call_in_context(id { :alt }) { topdown { constant_prop } }
      alt_env = @prop_env.pop
      conseq_env = @prop_env.pop
      @prop_env[-1] = alt_env.meet conseq_env
      build! :If[new_test,new_conseq,new_alt]
    elsif succeeds? { match :While[:test,:body] }
    elsif succeeds? { match :Assign[:Var[:name],:expr] }
      name = id { :name }
      expr = id { :expr }
      if simple_expr? { :expr }
        expr = @prop_env[name] = eval_simple_expr { :expr }
      elsif @prop_env.keys.include? name
        @prop_env.delete name
      end
      build! :Assign[:Var[:name],expr]
    elsif succeeds? { match :Var[:name] }
      name = id { :name }
      if @prop_env.keys.include? name
        build! @prop_env[-1][name]
      else
        id
      end
    else
      id
    end
  end

  def simple_expr?
    complex_expr = [:Array, :Not, :Or, :And, :Add, :Sub, :Mul, :Div, :Mod,
                    :Exp, :Equal, :GreaterThan, :LessThan, :GreaterEqual,
                    :LessEqual, :Defined]
    t = id
    if succeeds? { match :Lit[:num] } or succeeds? { match :True[] } or
       succeeds? { match :False[] } or succeeds? { match :Nil[] }
      true
    elsif succeeds? { match :Assign[:Var[:name],:val] } and 
            simple_expr? { :val }
      true
    elsif succeeds? { match :Var[:name] } and 
            @prop_env[-1].keys.include? id { :name }
      true
    elsif complex_expr.include? t.value
      for c in t.children
        return false if not simple_expr? { c }
      end
      true
    else
      false
    end
  end

  def eval_simple_expr
    eval_num = { :Add => :+, :Mul => :*, :Sub => :-, :Mod => :modulo, 
                 :Exp => :**, :GreaterThan => :>, :LessThan => :<, 
                 :GreaterEqual => :>=, :LessEqual => :<= }
    t = id
    if succeeds? { match :Defined[:expr] }
      arg = eval_simple_expr { :expr }
      if arg.value == :Nil then
        :True[]
      else
        :False[]
      end
    elsif succeeds? { match :Not[:expr] }
      arg = eval_simple_expr { :expr }
      if arg.value == :False
        :True[]
      else
        :False[]
      end
    elsif succeeds? { match :Or[:expr1, :expr2] }
      arg1 = eval_simple_expr { :expr1 }
      arg2 = eval_simple_expr { :expr2 }
      if arg1.value == :False and arg2.value == :False
        :False[]
      else
        :True[]
      end
    elsif succeeds? { match :And[:expr1, :expr2] }
      arg1 = eval_simple_expr { :expr1 }
      arg2 = eval_simple_expr { :expr2 }
      if arg1.value == :False or arg2.value == :False
        :False[]
      else
        :True[]
      end
    elsif succeeds? { match :Equal[:expr1, :expr2] }
      arg1 = eval_simple_expr { :expr1 }
      arg2 = eval_simple_expr { :expr2 }
      if arg1.value == arg2.value
        if arg1.value == :Lit and arg1.children[0] == arg1.children[1]
          :True[]
        elsif arg1.value != :Lit
          :True[]
        else
          :False[]
        end
      else
        :False[]
      end
    elsif succeeds? { match :Var[:name] }
      @prop_env[-1][id { :name }]
    elsif succeeds? { match :Assign[:Var[:name],:expr] }
      k = id { :name }
      v = eval_simple_expr { :expr }
      @prop_env[-1][k] = v
    elsif t.value == :Array
      items = t.children.map { |c| eval_simple_expr { c } }
      :Array[*items]
    elsif eval_num.keys.include? t.value
      args = t.children.map { |c| eval_simple_expr { c } }
      if args[0].value != :Lit or args[1].value != :Lit
        raise RubyWrite::Fail.new("Attempt to apply numeric operation to non numerica value(s): #{args[0].to_string}, #{args[1].to_string}")
      else
        v1 = args[0].children[0].to_num
        v2 = args[1].children[0].to_num
        res = v1.send eval_num[t.value], v2
        :Lit[res.to_s]
      end
    else
      raise RubyWrite::Fail.new("Unable to eval term: #{t.to_string}")
    end
  end
end
