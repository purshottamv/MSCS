require 'rubywrite/exceptions'
require 'rubywrite/node'
require 'rubywrite/basic'
require 'rubywrite/prettyprint'
require 'rubywrite/traversals'

module RubyWrite
  include RubyWrite::Basic
  include RubyWrite::PrettyPrint
  include RubyWrite::Traversals

  def self.included base
    base.extend RubyWrite::ReWriter::ClassMethods
    base.extend RubyWrite::Collectives::ClassMethods
  end
end
