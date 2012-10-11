require 'test/unit'
require 'rubywrite'

class DeadCodeElimination
  include RubyWrite
  def main n; ast, liveout = dce n, []; ast; end
  define_rw_rewriter :dce do
    rewrite :Rval[:var] { |n, live| [n, live | [lookup(:var)]] }
    rewrite :If[:t,:c,:a] { |n, live|
      (c, cl), (a, al) = [dce(lookup(:c), live), dce(lookup(:a), live)]
      t, live = dce lookup(:t), cl | al
      [:If[t,c,a], live] }
    rewrite :Assign[:Lval[:lhs],:rhs] { |n, live|
      if live.include? lookup(:lhs) then
        rhs, live = dce lookup(:rhs),live - [lookup(:lhs)]
        [build(:Assign[:Lval[:lhs],rhs]), live]
      else [nil, live] end }
    default { |n, live| 
      n.children, live = dce_exprs n.children, live unless n.is_a? String
      [n, live] }
  end
  define_rw_method :dce_exprs do |exprs, live|
    rstmts, live = exprs.reverse.inject [[],live] { |st, stmt|
      stmt, st[1] = dce stmt, st[1]; st[0] << stmt if stmt; st }
    [rstmts.reverse, live]
  end
end

class GenericRevDFA
  # little things to fake it.
  def self.define_rw_rpostorder name
    yield
  end
  def self.upon name, &blk
    @@me ||= {}
    @@me[name] = blk
  end
  define_rw_rpostorder :process do
    upon :Rval do |n, set| set.add n[0] end
    upon :If do |n, set| 
      analyze n[0], analyze(n[1], set) | analyze(n[2], set) 
    end
    upon :Assign do |n, set|
      set.remove n[0]
      analyze n[1], set
    end
    upon :While do |n, set| 
      analyze n[0], (fixed_point(set) { |set| analyze(n[1], set) })
    end
  end
end


class TestDCE < Test::Unit::TestCase
  def test_simple_dce
    res = DeadCodeElimination.run :Block[[:Assign[:Lval["x"],:Int["5"]],
                                          :Assign[:Lval["y"],:Int["6"]],
                                          :Assign[:Lval["z"],:Int["7"]],
                                          :Assign[:Lval["w"],
                                                  :Add[:Rval["x"],:Rval["y"]]],
                                          :Rval["w"]]]

    assert_equal :Block[[:Assign[:Lval["x"],:Int["5"]],
                         :Assign[:Lval["y"],:Int["6"]],
                         :Assign[:Lval["w"],:Add[:Rval["x"],:Rval["y"]]],
                         :Rval["w"]]], res

    res = DeadCodeElimination.run :Block[[:Assign[:Lval["x"],:Int["5"]],
                                          :Assign[:Lval["y"],:Int["6"]],
                                          :Assign[:Lval["z"],:Int["7"]],
                                          :Assign[:Lval["w"],
                                                  :Add[:Rval["x"],:Rval["y"]]],
                                          :If[:Test[],:Rval["z"],:Rval["w"]]]]
    assert_equal :Block[[:Assign[:Lval["x"],:Int["5"]],
                         :Assign[:Lval["y"],:Int["6"]],
                         :Assign[:Lval["z"],:Int["7"]],
                         :Assign[:Lval["w"],:Add[:Rval["x"],:Rval["y"]]],
                         :If[:Test[],:Rval["z"],:Rval["w"]]]], res
  end
end
