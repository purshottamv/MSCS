require 'rubywrite/node'
require 'rubywrite/basic'

module RubyWrite
  module PrettyPrint
    include RubyWrite::Basic

    def prettyprint (node, outfile=STDOUT)
      node.prettyprint outfile
      outfile.puts
    end
    def to_string (node)
      node.to_string
    end
  end

  class Node
    def prettyprint (outfile, indent='')
      outfile.print ":#{@value}["
      if (@children.length==0)
        outfile.print "]"
      else
        outfile.print "\n"
        comma = ' '
        @children.each do |c|
          outfile.print "#{indent+' '}#{comma}"
          c.prettyprint(outfile, indent+'  ')
          outfile.print "\n"
          comma = ','
        end
        outfile.print "#{indent}]"
      end
    end

    def to_string
      s = ":#{@value}["
      comma = ''
      @children.each {|c| s << "#{comma}#{c.to_string}"; comma=',' }
      s << "]"
      s
    end
  end

end

class String
  def prettyprint (outfile, indent='')
    outfile.print self.dump
  end
  def to_string
    self.dump
  end
end

class Symbol
  def prettyprint (outfile, indent='')
    outfile.print ":#{self.to_s}"
  end
  def to_string
    ":#{self.to_s}"
  end
end

class Array
  def prettyprint (outfile, indent='')
    outfile.print "["
    left = ""
    self.each {|x| outfile.print left; x.prettyprint(outfile, indent+' '); left = "\n"+indent+"," }
    outfile.print "\n#{indent}]"
  end
  def to_string
    s = "["
    comma = ""
    self.each {|x| s << comma+x.to_string; comma = "," }
    s+"]"
  end
end

class Proc
  def to_string
    "<Proc object:#{self}>"
  end
end


# We provide a catchall, to avoid runtime errors
class Object
  def prettyprint (outfile, indent='')
    outfile.print "#{self.to_s}"
  end
  def to_string
    "#{self.to_s}"
  end
end
