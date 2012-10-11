require 'rubywrite/exceptions'
module RubyWrite
  class Environment
    def initialize
      @env = [{}]
    end

    def []= (sym, val)
      raise Fail.new("RubyWrite::Environment[]= got invalid args: symbol (#{sym.to_string}), val (#{val.to_string})") if !(sym.instance_of?(Symbol) && val.valid_tree?)
      @env.last[sym] = val
    end

    def [] (sym)
      raise Fail.new("RubyWrite::Environment[] got invalid arg: symbol (#{sym.to_string})") if !(sym.instance_of?(Symbol))
      (@env.length-1).downto(0) {|i| return @env[i][sym] if @env[i][sym] }
      # If the symbol is not found, return nil instead of raising an error
      nil
    end

    # Sometimes we just want to tentatively extend the current environment.  This is true
    # for +match+ and block passed to +match+ or +build+.  A block passed to any other
    # method executes in that method's environment, because RubyWrite has no control over
    # either the method or the block.
    def grow
      @env.push({})
    end

    # Environment can be shrunk (i.e., the extension can be unrolled).  Useful if
    # match? fails.
    def shrink
      raise Error.new("RubyWrite::Environment::shrink: cannot shrink below current #{env.length}") if @env.length <= 1
      @env.pop
    end

    # Commit last extension merges the last extended environment with the previous one.
    # A redefinition in the extension overwrites any in the previous one.  This is not
    # necessary for correctness.  It is an optimization.
    def commit_last_extension
      raise Error.new("RubyWrite::Environment::commmit_last_extension: no extension to commit") if @env.length <= 1
      top = @env.pop
      top.each_pair {|key, val| self[key] = val }
    end

    # +match+ needs to know if a given symbol exists in the latest extended environment.
    # If a symbol was added while performing the current match then it must be in the
    # last environment and +match+ can performance "unification" with the tree bound to
    # it.  Otherwise, even though a lookup on the symbol might succeed, it shouldn't be
    # used for unification, since its value comes from an earlier binding in the
    # surrounding context.
    def lookup_last_env sym
      @env.last[sym]
    end

    def to_s
      "#{@env.length-1} extensions, #{@env[-1].length} symbols in the last extension"
    end

    def dump
      puts "="*20
      puts to_s
      count = 1
      @env.each do |e|
        puts "Env# #{count}"
        count += 1
        e.each_pair {|k, v| puts "\t#{k} => #{v.to_string}" }
      end
      puts "="*20
    end
  end
end
