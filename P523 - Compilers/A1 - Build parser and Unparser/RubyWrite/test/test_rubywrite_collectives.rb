require 'test/unit'
require 'rubywrite'

class TestRubyWriteCollective < Test::Unit::TestCase
  def test_define_rules
    assert_equal :For["i",:Int["10"],:Int["1"],
                   :Stmt[:AddAssign[:Var["x"],:Int["1"]]]],
      DefRulesTest.run(:For["i",:Int["1"],:Int["10"],
                         :Stmt[:AddAssign[:Var["x"],:Int["1"]]]])
    assert_equal :For["i",:Int["100"],:Int["10"],
                   :For["j",:Int["500"],:Int["50"],
                     :For["k",:Int["900"],:Int["90"],
                       :Stmt[:AddAssign[:Var["x"],:Int["1"]]]]]],
      DefRulesTest.run(:For["i",:Int["10"],:Int["100"],
                         :For["j",:Int["50"],:Int["500"],
                           :For["k",:Int["90"],:Int["900"],
                             :Stmt[:AddAssign[:Var["x"],:Int["1"]]]]]])
    assert_equal :For["i",:Int["100"],:Int["10"],
                   :Block[
                     :Stmt[:Assign[:Var["y"],:Int["1"]]],
                     :For["j",:Int["500"],:Int["50"],
                       :Block[
                         :Stmt[:Assign[:Var["z"],:Int["2"]]],
                         :For["k",:Int["900"],:Int["90"],
                           :Stmt[:AddAssign[:Var["x"],:Int["1"]]]]]]]],
      DefRulesTest.run(:For["i",:Int["10"],:Int["100"],
                         :Block[
                           :Stmt[:Assign[:Var["y"],:Int["1"]]],
                           :For["j",:Int["50"],:Int["500"],
                             :Block[
                               :Stmt[:Assign[:Var["z"],:Int["2"]]],
                               :For["k",:Int["90"],:Int["900"],
                                 :Stmt[:AddAssign[:Var["x"],:Int["1"]]]]]]]])
  end

  def test_fancy_rules
    assert_equal :Assign[:VarField["x"],:Fixnum["1"]],
      FancyRulesTest.run(:Assign[:VarField["x"],:Fixnum["1"]])
    assert_equal :Call[:VarRef["o"],".","a=",[:Fixnum["1"]]],
      FancyRulesTest.run(:Assign[:Field[:VarRef["o"],".","a"],:Fixnum["1"]])
  end

  class FancyRulesTest
    include RubyWrite::Collectives

    def main n
      fancy_rewrite n
    end

    define_rw_rewriter :fancy_rewrite do
      rewrite :Assign[:Field[:expr,:op,:id],:rhs] do |n|
        build :Call[:expr,:op,"#{lookup(:id)}=",[:rhs]]
      end
      default { |n| n }
    end
  end

  class DefRulesTest
    include RubyWrite::Collectives

    def main n
      reverser n
    end

    define_rw_rewriter :reverser do 
      rewrite :For[:var,:start,:end,:stmt] do
        build :For[:var,:end,:start,reverser(lookup(:stmt))]
      end

      default do |n|
        case n
        when RubyWrite::Node then 
          n.children.map! { |c| reverser c }
          n
        when Array then n.map { |c| reverser c }
        when String then n
        end
      end
    end
  end
end

