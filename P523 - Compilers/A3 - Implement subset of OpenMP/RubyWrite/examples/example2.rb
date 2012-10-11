require 'rubywrite'

class Transformer < RubyWrite::ReWriter
  include RubyWrite::Traversals
  include RubyWrite::PrettyPrint
  def main (node)
    puts "Given code:"
    puts node.to_string
    puts "-"*20
    puts

    puts "CALLING ONE!"
    #(one!(node) {|n| change_some_nodes! n}).prettyprint
    (node.one! apply{|n| change_some_nodes! n}).prettyprint
    puts "\n", "-"*20

    puts "\nCALLING BOTTOMUP?"
    #bottomup?(node) {|n| puts n.to_string; true}
    node.bottomup? apply{|n| puts n.to_string; true}
    puts "-"*20

    puts "\nCALLING TOPDOWN!"
    #topdown!(node) {|n| puts n.to_string; n}
    node.topdown! apply{|n| puts n.to_string; n}
    puts "-"*20

    puts "\nUSING alltd TO REVERSE ARRAYS"
    #(alltd!(node) {|n| n.reverse if n.class == Array}).prettyprint
    (node.alltd! apply{|n| n.reverse if n.class == Array}).prettyprint
    puts "\n", "-"*20

    puts "\nUSING alltd TO CONVERT ASSIGNMENTS IN AN ARRAY (succeeds) AND A STRING (fails)"
    #(alltd!(node) {|n| convert_assigns!(n)}).prettyprint
    ([:Assign['x','10'], :Assign['y',:Binop['+','x','20']]].alltd! apply{|n| convert_assigns!(n)}).prettyprint
    puts
    ('some string'.alltd! apply{|n| convert_assigns!(n)}).prettyprint
    puts "\n", "="*20

    puts "\nUSING topdown? TO LOOK FOR Binop"
    if (node.alltd? apply{|n| n.match? :Binop[:_, :_, :_], apply{|f| set! :call, f}})
      puts "Found Binop: \"#{lookup(:call).to_string}\""
    else
      puts "Found no Binop"
    end
  end

  def change_some_nodes! (node)
    case node
    when '10'
      'ten'
    when '11'
      'eleven'
    end
  end

  def print_node
    puts id.to_string
    @curT
  end

  def reverse_array! (node)
    node.reverse if node.class == Array
  end

  def convert_assigns! (node)
    build :Let[:a, :b] if node.match? :Assign[:a, :b], apply{}
  end
end

Transformer.run(:For['i', '1', '10', [:Assign['x','10'], :Assign['y','20'], :Assign['z',:Binop['*','x','y']]]])
