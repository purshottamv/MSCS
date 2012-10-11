require 'rubywrite/environment'
require 'rubywrite/code'
require 'rubywrite/rewriter'
require 'rubywrite/standard'
require 'rubywrite/collectives'

module RubyWrite
  module Basic
    include RubyWrite::ReWriter
    include RubyWrite::Collectives

    def self.included base
      base.extend RubyWrite::ReWriter::ClassMethods
      base.extend RubyWrite::Collectives::ClassMethods
    end
  end
end
