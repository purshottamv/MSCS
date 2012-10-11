require 'rubywrite/node'
require 'rubywrite/environment'
require 'rubywrite/code'

module RubyWrite
  module ReWriter
    module ClassMethods
      def define_rw_method name, &blk
        define_method name do |*args|
          begin
            saved_env = @env
            @env = Environment.new
            instance_exec *args, &blk
          ensure
            @env = saved_env
          end
        end
      end

      def run node
        inst = self.new
        if inst.respond_to? :main
          inst.env = Environment.new
          inst.main node
        elsif inst.respond_to? :main!
          inst.main! node
        else
          raise RubyWrite::Fail,
            'RubyWrite: A "main" or "main!" method must be defined.'
        end
      end
    end

    def self.included base
      base.extend ClassMethods
    end

    attr_accessor :env

    # apply wraps the given block in a Code object
    def apply &blk
      Code.new(self, &blk)
    end

    # explicitly set a Symbol binding
    def set! sym, node
      @env[sym] = node
    end

    # try invokes the given block with the given arguments and commits
    # the environment only if the block returns non-nil (or non false)
    def try *args, &blk
      @env.grow
      if t = blk.call(*args)
        @env.commit_last_extension
      else
        @env.shrink
      end
      t
    end

    # used by the RubyWrite::Collectives library to apply a block 
    # to the current node after the pattern has been matched.
    def match_and_exec pattern, node, *args, &blk
      saved_env = @env
      @env = Environment.new
      match! pattern, node if pattern
      instance_exec node, *args, &blk
    ensure
      @env = saved_env
    end
  end
end


module RubyWrite
  class Node
    # override the default shallow-copy dclone method
    def dclone
      children = []
      @children.each {|c| children << c.dclone }
      n = Node.new(@value.dclone, children)
    end
  end
end

class Array
  def dclone
    collect { |x| x.dclone }
  end
end

class String
  def dclone
    String.new(self)
  end
end

class Symbol
  def dclone
    self
  end
end
