require 'rubywrite'

class Transformer < RubyWrite::ReWriter
  include RubyWrite::PrettyPrint
  def is_valid_body node
    if (succeeds? {match :Body[:x]})
      build! :Body[:x]
    else
      raise RubyWrite::Fail.new('Not a valid body')
    end
  end
  def reverse_body node
    build :Body[:r] if (match?(:For[:_, :_, :_, :Body[:stmts]]) { set! :r, :stmts.apply{reverse_array} })
  end

  def reverse_array node
    puts "reverse_array arg: id: #{id.to_string}"
    build! id.reverse
  end
  def random_builds node
    # random builds
    build! :Let['x', '10']
    build! :Repeat['this body', :Until[:Equal['x', 'true']]]
  end
  def main node
    puts "INITIAL TREE"
    puts node.to_string
    node.prettyprint
    if node.match? {match(:For[:idx, :l, :u, :Body[[:firstStmt, :FunctionCall[:fn, [:arg1, :arg2]]]]])}
      puts "\nMATCH SUCCEEDED"
      build! :For[:idx, :u, :l, :Body[[:firstStmt, :FunctionCall[:fn, [:arg1, :arg2]], :Assign['y', '-4']]]]
    else
      puts "\nMATCH FAILED"
    end
    puts to_string
    prettyprint

    puts
    case curT
    when :For[:a, :b, :c] then puts "Case For with 3 children"
    when :For[:a, :b, :c, :d] then puts "Case For with 4 children"
    else puts "Nothing matched"
    end

    # The following illustrates congurence and projection, with a rather convoluted example
    puts "\nMATCH AND BUILD WITH UNUSUAL SYNTAX AND CONTRIVED REVERSAL"
    match(:For[:i, :l, :u, :b]) {:b.lookup.apply { is_valid_body }}
    puts to_string
    build!(:StmtList[:Assign[:i,:l], :While[:Cond[:LessThan[:i, :u]], :b]]) {:b <= reverse_body}
    puts "\tCHANGES TO"
    prettyprint

    puts "\n:b is bound to: #{-:b}"

    # puts "\nSOME RANDOM BUILDS WITH A USER-LEVEL METHOD CALLED WITH A DEBUG BLOCK"
    # random_builds do
    #   puts "A block can also be used for debugging ..."
    #   puts "... it does not have to result in a valid term ..."
    #   puts "... in which case, it's executed, but its result is ignored."
    # end
    # prettyprint
    # 
    # # In fact, the reversal is better done as follows
    # puts "\nDOING THE REVERSAL THE 'RIGHT' WAY"
    # ## First, build a new term
    # build! :Body[[:FunctionCall['fact', :Args['10']], :Assign['x', '10'], :Assign['z', 'some string']]]
    # puts to_string
    # match(:Body[:lst]) { :r <= :lst.invoke(:reverse) }
    # build! :Body[:r]
    # puts "\tCHANGES TO"
    # prettyprint
    # 
    # # We can pass an explicit term, instead of working with the implicit "current" term
    # # And, do neat things with message invokation
    # puts "\nDOING SOME RANDOM OPERATIONS JUST TO EXPLORE THE SYNTAX"
    # puts "Using Symbol#invoke: #{:r.invoke(:to_string)}"
    # puts "Building with a complicated expression:"
    # build!(:Body[:x]) {:x <= [[:First['x', 'b'], 'second', '3'].apply! { reverse_array },  :r.invoke(:reverse)].flatten}
    # prettyprint
  end
end
Transformer.run(:For['i', '10','1', :Body[[:Assign['x','10'], :FunctionCall['print', ['x', '10']]]]])
