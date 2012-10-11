module RubyWrite
  # Fail is raised upon failure of an operation, but not due to an error condition.
  class Fail < Exception
    attr_reader :message
    def initialize (msg="")
      @message = msg+"\n\tStack trace:\n\t\t|- #{caller.join("\n\t\t|- ")}"
    end
    def to_s
      "#{super}: #{@message}"
    end
  end

  # Error is raised when an internal unexpected error occurs, e.g., a mal-formed tree.
  class Error < Exception
    attr_reader :message
    def initialize (msg)
      @message = msg+"\n\tStack trace:\n\t\t|- #{caller.join("\n\t\t|- ")}"
    end
    def to_s
      "#{super}: #{@message}"
    end
  end
end
