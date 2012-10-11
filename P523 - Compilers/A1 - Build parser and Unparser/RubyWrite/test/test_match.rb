require 'rubywrite'
require 'test/unit'

class TestRubyWriteMatch < Test::Unit::TestCase
  def test_match
    t = TestTransform.new
    m = t.match_node(:For['i', '1', '10', 'body'])
    assert_not_nil m
    i, l, u, b = m
    assert_equal('i', i)
    assert_equal('1', l)
    assert_equal('10', u)
    assert_equal('body', b)
    assert_nil t.fail_match(:For['i', '1', '10', 'body'])
  end

  class TestTransform 
    include RubyWrite
    define_rw_method :match_node do |n|
      if match?:For[:i, :l, :u, :b], n
        [lookup(:i), lookup(:l), lookup(:u), lookup(:b)]
      else
        nil
      end
    end

    define_rw_method :fail_match do |n|
      if match? :For[:i,:l,:u], n
        [lookup(:i), lookup(:l), lookup(:u)]
      else
        nil
      end
    end
  end
end
