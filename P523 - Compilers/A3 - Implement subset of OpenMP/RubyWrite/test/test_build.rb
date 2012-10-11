require 'rubywrite'
require 'test/unit'

class TestRubyWriteBuild < Test::Unit::TestCase
  def test_build
    assert_equal :For['i','10','1','body'], Rev.run(:For['i','1','10','body'])
  end

  class Rev
    include RubyWrite

    define_rw_method :main do |n|
      if match? :For[:i,:l,:u,:b], n
        build :For[:i,:u,:l,:b]
      else
        nil
      end
    end
  end
end
