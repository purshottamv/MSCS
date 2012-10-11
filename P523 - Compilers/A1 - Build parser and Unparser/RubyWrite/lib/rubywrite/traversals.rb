require 'rubywrite/basic'

module RubyWrite
  module Traversals
    include RubyWrite::Basic

    def all! (node, &blk)
      return node if node.numChildren <= 0
      new_children = []
      node.each_child do |c|
        if (t = try(c, &blk))
          new_children << t
        else
          raise Fail.new("RubyWrite::all! failed on node \"#{c.to_string}\"")
        end
      end
      node.children = new_children
      node
    end

    def all? (node, &blk)
      return true if node.numChildren <= 0
      node.each_child {|c| return false if !try(c, &blk) }
      true
    end

    def one! (node, &blk)
      return node if node.numChildren <= 0
      node.each_child_with_index do |c, i|
        if (t = try(c, &blk))
          node.children[i] = t
          return node
        end
      end
      raise Fail.new("RubyWrite::one! failed on node \"#{node.to_string}\"")
    end

    def one? (node, &blk)
      return true if node.numChildren <= 0
      node.each_child {|c| return true if try(c, &blk) }
      false
    end

    def alltd! (node, &blk)
      if (t = try(node, &blk))
        t
      else
        all!(node) {|n| alltd!(n, &blk)}
      end
    end

    def alltd? (node, &blk)
      if try(node, &blk)
        true
      else
        all?(node) {|n| alltd?(n, &blk)}
      end
    end

    def topdown! (node, &blk)
      if (n = try(node, &blk))
        n.each_child_with_index do |c, i|
          if (t = topdown!(c, &blk))
            n.children[i] = t
          else
            raise Fail.new("RubyWrite::topdown! failed on node \"#{c}\"")
          end
        end
        n
      else
        raise Fail.new("RubyWrite::topdown! failed on node \"#{n.to_string}\"")
      end
    end

    def topdown? (node, &blk)
      return false if !try(node, &blk)
      node.each_child {|c| return false if !topdown?(c, &blk) }
      true
    end

    def bottomup! (node, &blk)
      node.each_child_with_index do |c, i|
        if (t = bottomup!(c, &blk))
          node.children[i] = t
        else
          raise Fail.new("RubyWrite::bottomup! failed on node \"#{c.to_string}\"")
        end
      end
      if (n = try(node, &blk))
        n
      else
        raise Fail.new("RubyWrite::bottomup! failed on node \"#{n.to_string}\"")
      end
    end

    def bottomup? (node, &blk)
      node.each_child {|c| return false if !bottomup?(c, &blk) }
      return false if !try(node, &blk)
      true
    end
  end

  class Node
    def all! (code)
      code.xer.all!(self) {|*a| code.call *a}
    end

    def all? (code)
      code.xer.all?(self) {|*a| code.call *a}
    end

    def one! (code)
      code.xer.one!(self) {|*a| code.call *a}
    end

    def one? (code)
      code.xer.one?(self) {|*a| code.call *a}
    end

    def alltd! (code)
      code.xer.alltd!(self) {|*a| code.call *a}
    end

    def alltd? (code)
      code.xer.alltd?(self) {|*a| code.call *a}
    end

    def topdown! (code)
      code.xer.topdown!(self) {|*a| code.call *a}
    end

    def topdown? (code)
      code.xer.topdown?(self) {|*a| code.call *a}
    end

    def bottomup! (code)
      code.xer.bottomup!(self) {|*a| code.call *a}
    end

    def bottomup? (code)
      code.xer.bottomup?(self) {|*a| code.call *a}
    end
  end
end


class Array
  def all! (code)
    code.xer.all!(self) {|*a| code.call *a}
  end

  def all? (code)
    code.xer.all?(self) {|*a| code.call *a}
  end

  def one! (code)
    code.xer.one!(self) {|*a| code.call *a}
  end

  def one? (code)
    code.xer.one?(self) {|*a| code.call *a}
  end

  def alltd! (code)
    code.xer.alltd!(self) {|*a| code.call *a}
  end

  def alltd? (code)
    code.xer.alltd?(self) {|*a| code.call *a}
  end

  def topdown! (code)
    code.xer.topdown!(self) {|*a| code.call *a}
  end

  def topdown? (code)
    code.xer.topdown?(self) {|*a| code.call *a}
  end

  def bottomup! (code)
    code.xer.bottomup!(self) {|*a| code.call *a}
  end

  def bottomup? (code)
    code.xer.bottomup?(self) {|*a| code.call *a}
  end
end


class String
  def all! (code)
    code.xer.all!(self) {|*a| code.call *a}
  end

  def all? (code)
    code.xer.all?(self) {|*a| code.call *a}
  end

  def one! (code)
    code.xer.one!(self) {|*a| code.call *a}
  end

  def one? (code)
    code.xer.one?(self) {|*a| code.call *a}
  end

  def alltd! (code)
    code.xer.alltd!(self) {|*a| code.call *a}
  end

  def alltd? (code)
    code.xer.alltd?(self) {|*a| code.call *a}
  end

  def topdown! (code)
    code.xer.topdown!(self) {|*a| code.call *a}
  end

  def topdown? (code)
    code.xer.topdown?(self) {|*a| code.call *a}
  end

  def bottomup! (code)
    code.xer.bottomup!(self) {|*a| code.call *a}
  end

  def bottomup? (code)
    code.xer.bottomup?(self) {|*a| code.call *a}
  end
end
