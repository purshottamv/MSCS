require 'rubywrite'
require 'test/unit'

class TestRubyWriteComplexMatch < Test::Unit::TestCase
 def test_match
   n = :For['i','10','1',:Body[[:Assign['x','10'],
     :FunctionCall['print',['x','10']]]]]
   t = TestTransform.new
   m = t.match_for_assign n
   assert_not_nil m
   i, l, u, var, val, stmt, param = m
   assert_equal('i', i)
   assert_equal('10', l)
   assert_equal('1', u)
   assert_equal('x', var)
   assert_equal('10', val)
   assert_equal('print', stmt)
   assert_equal(["x","10"], param)

   m = t.match_build_match n
   assert_not_nil m
   i, l, u, b = m
   assert_equal('i', i)
   assert_equal('1', l)
   assert_equal('10', u)
   assert_equal(:Body[[:FunctionCall['print',['x','10']],:Assign['x','10']]], b)
 end

 class TestTransform
   include RubyWrite

   define_rw_method :match_for_assign do |n|
     if match? :For[:i,:l,:u,:Body[[:Assign[:var,:val],
       :FunctionCall[:stmt,:param]]]], n
       [lookup(:i), lookup(:l), lookup(:u), lookup(:var), lookup(:val),
         lookup(:stmt), lookup(:param)]
     else
       nil
     end
   end

   define_rw_method :match_build_match do |n|
     unless match?(:For[:i,:l,:u,:Body[[:Assign[:var,:val],
                   :FunctionCall[:stmt,:param]]]], n)
       return nil
     end

     n = build(:For[:i,:u,:l,:Body[[:FunctionCall[:stmt,:param],
               :Assign[:var,:val]]]])
     if match? :For[:i,:l,:u,:b], n
       [lookup(:i),lookup(:l),lookup(:u),lookup(:b)]
     else
       nil
     end
   end
 end
end
