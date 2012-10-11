require 'rubywrite'
require 'test/unit'

class TestRubyWriteBadMatch < Test::Unit::TestCase
  class TestMatch
    include RubyWrite
    define_rw_method(:matches?) { |pattern, n| match?(pattern, n) }
  end

  def test_badMatch
    t = TestMatch.new
    assert !t.matches?(:For[:idx,:l,:u,:body], :For['i','1','10'])
    assert t.matches?(:For[:idx,:l,:u], :For['i','1','10'])
    assert !t.matches?(:Repeat['Body',:Until[:Equal['x','true']]],
                       :For['i','1','10'])
  end
end
