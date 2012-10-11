require 'rubywrite'
require 'test/unit'

class TestRubyWriteTravBasic < Test::Unit::TestCase
  class TestTransform
    include RubyWrite

    define_rw_method :do_one do |n|
      ls = []
      one!(n) { |n| ls << n.to_string ; n }
      ls
    end

    define_rw_method :do_all do |n| 
      ls = []
      all!(n) { |n| ls << n.to_string ; n }
      ls
    end

    define_rw_method :do_alltd do |n|
      ls = []
      alltd!(n) { |n| ls << n.to_string ; n }
      ls
    end

    define_rw_method :do_bottomup do |n|
      ls = []
      bottomup!(n) { |n| ls << n.to_string ; n }
      ls
    end

    define_rw_method :do_topdown do |n|
      ls = []
      topdown!(n) { |n| ls << n.to_string ; n }
      ls
    end
  end

  def test_topdown
    t = TestTransform.new
    ls = t.do_topdown(:For['i','1','10',[:Assign['x','10'],:Assign['y','20'],
                      :Assign['z',:Binop['*','x','y']]]])
    assert_equal 17, ls.length
    assert_equal(":For[\"i\",\"1\",\"10\",[:Assign[\"x\",\"10\"]," \
      ":Assign[\"y\",\"20\"],:Assign[\"z\",:Binop[\"*\",\"x\",\"y\"]]]]", ls[0])
    assert_equal('"i"',ls[1])
    assert_equal('"1"',ls[2])
    assert_equal('"10"',ls[3])
    assert_equal("[:Assign[\"x\",\"10\"],:Assign[\"y\",\"20\"]," \
                 ":Assign[\"z\",:Binop[\"*\",\"x\",\"y\"]]]", ls[4])
    assert_equal(":Assign[\"x\",\"10\"]", ls[5])
    assert_equal('"x"', ls[6])
    assert_equal('"10"',ls[7])
    assert_equal(":Assign[\"y\",\"20\"]", ls[8])
    assert_equal('"y"', ls[9])
    assert_equal('"20"', ls[10])
    assert_equal(":Assign[\"z\",:Binop[\"*\",\"x\",\"y\"]]", ls[11])
    assert_equal('"z"', ls[12])
    assert_equal(":Binop[\"*\",\"x\",\"y\"]", ls[13])
    assert_equal('"*"', ls[14])
    assert_equal('"x"', ls[15])
    assert_equal('"y"', ls[16])
  end

  def test_all
    t = TestTransform.new
    ls = t.do_all(:For['i','1','10',[:Assign['x','10'],:Assign['y','20'],
                  :Assign['z',:Binop['*','x','y']]]])
    assert_equal 4, ls.length
    assert_equal('"i"', ls[0])
    assert_equal('"1"', ls[1])
    assert_equal('"10"', ls[2])
    assert_equal("[:Assign[\"x\",\"10\"],:Assign[\"y\",\"20\"]," \
                 ":Assign[\"z\",:Binop[\"*\",\"x\",\"y\"]]]", ls[3])
  end

  def test_alltd
    t = TestTransform.new
    ls = t.do_alltd(:For['i','1','10',[:Assign['x','10'],:Assign['y','20'],
                    :Assign['z',:Binop['*','x','y']]]])
    assert_equal 1, ls.length
    assert_equal(ls[0], ":For[\"i\",\"1\",\"10\",[:Assign[\"x\",\"10\"]," \
      ":Assign[\"y\",\"20\"],:Assign[\"z\",:Binop[\"*\",\"x\",\"y\"]]]]")
  end

  def test_bottomup
    t = TestTransform.new
    ls = t.do_bottomup(:For['i','1','10',[:Assign['x','10'],:Assign['y','20'],
                       :Assign['z',:Binop['*','x','y']]]])
    assert_equal 17, ls.length
    assert_equal('"i"', ls[0])
    assert_equal('"1"', ls[1])
    assert_equal('"10"', ls[2])
    assert_equal('"x"', ls[3])
    assert_equal('"10"', ls[4])
    assert_equal(":Assign[\"x\",\"10\"]", ls[5])
    assert_equal('"y"', ls[6])
    assert_equal('"20"', ls[7])
    assert_equal(":Assign[\"y\",\"20\"]", ls[8])
    assert_equal('"z"', ls[9])
    assert_equal('"*"', ls[10])
    assert_equal('"x"', ls[11])
    assert_equal('"y"', ls[12])
    assert_equal(":Binop[\"*\",\"x\",\"y\"]", ls[13])
    assert_equal(":Assign[\"z\",:Binop[\"*\",\"x\",\"y\"]]", ls[14])
    assert_equal("[:Assign[\"x\",\"10\"],:Assign[\"y\"," \
      "\"20\"],:Assign[\"z\",:Binop[\"*\",\"x\",\"y\"]]]", ls[15])
    assert_equal(":For[\"i\",\"1\",\"10\",[:Assign[\"x\"," \
      "\"10\"],:Assign[\"y\",\"20\"],:Assign[\"z\",:Binop[\"*\",\"x\"," \
      "\"y\"]]]]", ls[16])
  end

  def test_one
    t = TestTransform.new
    ls = t.do_one(:For['i','1','10',[:Assign['x','10'],:Assign['y','20'],
                  :Assign['z',:Binop['*','x','y']]]])
    assert_equal 1, ls.length
    assert_equal '"i"', ls[0]
  end
end
