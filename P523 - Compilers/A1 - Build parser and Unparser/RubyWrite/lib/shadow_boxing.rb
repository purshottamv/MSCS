require 'rubygems'
require 'rubywrite'
require 'prettyprint'

class ShadowBoxing
  attr_accessor :rules

  def initialize &block
    @rules = {}
    instance_eval(&block)
  end

  def unparse_node node
    Boxer.new process(node)
  end

  def h opt, *nodes
    opt[:hs] ||= 0

    :H[opt,process(nodes)]
  end

  def h_star opt, sep, *nodes
    opt[:hs] ||= 0

    if nodes == [] then
      n_star = []
    else
      n_star = nodes[0..-2].inject([]) do |acc, v|
        acc << process(v) << sep 
      end << process(nodes[-1])
    end

    :H[opt,n_star]
  end

  def v opt, *nodes
    opt[:vs] ||= 0
    opt[:is] ||= 0

    :V[opt,process(nodes)]
  end

  def hv opt, *nodes
    opt[:hs] ||= 0
    opt[:vs] ||= 0
    opt[:is] ||= 0

    :HV[opt,process(nodes)]
  end

  def rule node_type, &body_proc
    @rules[node_type] = body_proc
  end

  def process x
    # If it is a node, assume we need to process it.
    if x.instance_of? RubyWrite::Node
      if rule = @rules[x.value]
        rule.call *x.children
      # ignore or boxers unless someone's already written a rule for them
      elsif [:H, :V, :HV].include? x.value
        x
      else
        puts "ShadowBoxing: Warning: unknown node type: #{x.value} -- #{x}"
      end
    # if it is an array, map over the array
    elsif x.respond_to? :to_ary
      x.map { |y| process y }
    # otherwise assume we've got a string or something that converts
    # nicely to a string
    else
      unless [String, Fixnum, Symbol].include? x.class
        puts "ShadowBoxing: Not sure what to do with this: #{x.class} -- #{x}"
      end
      x.to_s
    end
  end
end

class Boxer
  attr_accessor :box_node, :indent, :width
  def initialize box_node = nil, indent = 0, width = 78
    @box_node, @indent, @width = box_node, indent, width
  end

  def to_s width = nil
    process @box_node
  end

  def process n
    if n.instance_of? RubyWrite::Node
      case n.value
      when :H
        process_h *n.children
      when :V
        process_v *n.children
      when :HV
        process_hv *n.children
      end
    else
      n.to_s
    end
  end

  def process_h opt, children 
    space = " " * opt[:hs]
    strs = children.map { |c| process(c) }
    strs.join space
  end

  def process_v opt, children 
    @indent += opt[:is]
    in_space = " " * @indent
    strs = children.map { |c| process(c).rstrip }
    @indent -= opt[:is]
    strs.join "\n" + in_space
  end

  def process_hv opt, children 
    @indent += opt[:is]
    space = " " * opt[:hs]
    in_space = " " * @indent
    strs = children.map { |c| process(c) }
    lines = []
    line = ""
    strs.each do |s|
      if (line.length + space.length + s.length) >= @width then
        lines << line.rstrip
        line = in_space + s
      else
        line += space + s
      end
    end
    @indent -= opt[:is]
    lines.join "\n"
  end
end
