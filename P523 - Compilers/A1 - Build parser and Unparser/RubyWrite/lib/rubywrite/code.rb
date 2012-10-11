module RubyWrite
  class Code
    attr_reader :xer, :code

    def initialize (xformer, &blk)
      @xer = xformer
      @code = blk
    end

    def call (*args)
      @code.call *args
    end

    def env
      @xer.env
    end
  end
end
