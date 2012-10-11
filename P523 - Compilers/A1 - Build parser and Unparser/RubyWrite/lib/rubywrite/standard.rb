module RubyWrite
  # ReWriter class has some standard operations defined.  All user classes get these without including any modules.
  module ReWriter
    # If match? succeeds then the new enviroment is merged back into the caller's.
    # Otherwise, caller's environment is unaffected.  If a block is given it is
    # executed after (and if) the match succeeds.  Block is sometimes convenient
    # for a bit of post-processing.
    def match? (pat, tree)
      @env.grow
      if pat.pattern_match?(tree, @env)
        @env.commit_last_extension
        yield(tree) if block_given?
        return true
      else
        @env.shrink
        return false
      end
    end

    # Sometimes we want to fail if match does not succeed.  match! serves that purpose.
    def match! (pat, tree)
      if pat.pattern_match?(tree, @env)
        yield(tree) if block_given?
      else
        raise Fail.new "Failed to match pattern #{pat.to_string} with tree #{tree.to_string}"
      end
    end


    # build first executes the given block and then builds the tree from the given spec.
    # The block is sometimes convenient to do a bit of pre-processing.
    def build (spec)
      yield if block_given?
      spec.build_term @env
    end

    # lookup is to lookup a tree binding
    def lookup (s)
      if s
        @env[s]
      else
        raise Fail.new("RubyWrite::lookup: Attempting to lookup nil value")
      end
    end
  end
end


class Object
  # Default pattern_match? raises an error.
  def pattern_match? (term, env)
    raise RubyWrite::Error.new("match: pattern matching not defined on type #{self.class}")
  end

  # Default build_term raises an error.
  def build_term (env)
    raise RubyWrite::Error.new("build: build not defined on type #{self.class}")
  end
end


class Symbol
  def pattern_match? (term, env)
    if (t = env.lookup_last_env(self))
      t.pattern_match? term, env
    else
      env[self] = term if self != :_
      true
    end
  end

  def build_term (env)
    env[self]
  end

  def match? (pat, code)
    code.xer.match?(pat, self) {code.call}
  end

  def match! (pat, code)
    code.xer.match!(pat, self) {code.call}
  end
end


class String
  def pattern_match? (term, env)
    if (!term.instance_of?(String) || (self != term)); false; else true; end
  end

  def build_term (env)
    self
  end

  def match? (pat, code)
    code.xer.match?(pat, self) {|*a| code.call *a}
  end

  def match! (pat, code)
    code.xer.match!(pat, self) {|*a| code.call *a}
  end
end


class Array
  def pattern_match? (term, env)
    return false if (!term.instance_of?(Array) || (self.length != term.length))
    0.upto(self.length-1) {|i| return false if !self[i].pattern_match?(term[i], env) }
    true
  end

  def build_term (env)
    inject([]) {|tree, element| tree << element.build_term(env) }
  end

  def match? (pat, code)
    code.xer.match?(pat, self){|*a| code.call *a}
  end

  def match! (pat, code)
    code.xer.match!(pat, self){|*a| code.call *a}
  end
end


module RubyWrite
  class Node
    def pattern_match? (term, env)
      term.instance_of?(Node) && (@value == term.value) && @children.pattern_match?(term.children, env)
    end

    def build_term (env)
      Node.new @value, @children.build_term(env)
    end

    def match? (pat, code)
      code.xer.match?(pat, self) {|*a| code.call *a}
    end

    def match! (pat, code)
      code.xer.match!(pat, self) {|*a| code.call *a}
    end
  end
end
