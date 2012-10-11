require 'test/unit'
require 'rubywrite'

class TestRubyWriteNode < Test::Unit::TestCase
  def test_equal
    assert_equal :Foo[], :Foo[]
    assert_equal :Foo["a", "b"], :Foo["a", "b"]
    assert_equal :Foo[:Bar[], :Baz["a", "b", "c", "d"]],
      :Foo[:Bar[], :Baz["a", "b", "c", "d"]]
    assert_equal :Foo[:Baz[[:Var["a"], :Var["b"], :Var["c"], :Var["d"]]]],
      :Foo[:Baz[[:Var["a"], :Var["b"], :Var["c"], :Var["d"]]]]
    assert_not_equal :Test["a", "b"], :Test[]
    assert_not_equal :Test[], :Test["a", "b"]
    assert_not_equal :Test[["a", "b"]], :Test[["c", "d"]]
  end

  def test_attributes
    n = :Test[]
    assert_equal nil, n.get_attr(:line)
    n.set_attr :line, 10
    assert_equal 10, n.get_attr(:line)

    assert_equal nil, n.get_attr(:column)
    assert_equal 10, n.get_attr(:line)
    n.set_attr :column, 20
    assert_equal 20, n.get_attr(:column)
    assert_equal 10, n.get_attr(:line)

    n.set_attr :line, 50
    assert_equal 50, n.get_attr(:line)
    assert_equal 20, n.get_attr(:column)
  end
end

