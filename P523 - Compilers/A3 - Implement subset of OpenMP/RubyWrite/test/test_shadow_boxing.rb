require 'test/unit'
require 'shadow_boxing'

class TestShadowBoxing < Test::Unit::TestCase
  def test_unparse
    boxer = ShadowBoxing.new do 
      rule :If do |t, c, a|
        v({}, v({:is => 2}, h({:hs => 1}, "if", t, "then"), c), a, "end")
      end
      rule :Call do |recv, meth, args| 
        case meth
        when :==, :+, :-, :*, :/ 
          if args.value == :OptionalArgs then
            h({:hs => 1}, recv, meth,
              h_star({:hs => 1}, ",", *args.children[0].children[0]))
          else
            h({}, recv, meth)
          end
        else
          h({}, recv, ".", meth, args)
        end
      end
      rule :Vcall do |var_meth| var_meth end
      rule :OptionalArgs do |ary| 
        h({}, "(", h_star({:hs => 1}, ",", *ary.children[0]), ")")
      end
      rule :Lit do |lit| lit end
      rule :Fcall do |meth, args| h({}, meth, args) end
      rule :Str do |str| h({:hs => 0}, "\"", str, "\"") end
      rule :OptionalElse do |body| v({:is => 2}, "else", body) end
      rule :None do "" end
    end

    box = boxer.unparse_node :If[:Call[:Vcall[:x],:==,
                              :OptionalArgs[:Array[[:Lit[5]]]]],
                    :Fcall[:puts,:OptionalArgs[:Array[[:Str['foo']]]]],
                  :OptionalElse[
                    :Fcall[:put,:OptionalArgs[:Array[[:Str['bar']]]]]]]
    assert "if x == 5 then\n  puts(\"foo\")\nelse\n  puts(\"bar\")\nend",
      box.to_s
  end
end
