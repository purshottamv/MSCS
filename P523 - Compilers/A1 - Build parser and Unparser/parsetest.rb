cwd = File.dirname(__FILE__)
$:.unshift cwd, cwd + '/RubyWrite/lib'

require 'PCParse/pcparser'


# We need to first create a parser object
parser = PCParser.new

# The parser can parse from an array consisting of [token, value] pairs
t = parser.parse_array [[:INT, 'int'],
                        [:IDENTIFIER, 'foo'],
                        ['(', '('],
                        [')', ')'],
                        ['{', '{'],
                        ['}', '}'],
                        [:DOUBLE, 'double'],  # double bar (int x, double z[])
                        [:IDENTIFIER, 'bar'],
                        ['(', '('],
                        [:INT, 'int'],
                        [:IDENTIFIER, 'x'],
                        [',', ','],
                        [:DOUBLE, 'double'],
                        [:IDENTIFIER, 'z'],
                        ['[', '['],
                        [']', ']'],
                        [')', ')'],
                        ['{', '{'],
                        [:IDENTIFIER, 'y'],  # y = x + 1;
                        ['=', '='],
                        [:IDENTIFIER, 'x'],
                        ['+', '+'],
                        [:INT_NUM, '1'],
                        [';', ';'],
                        [:IDENTIFIER, 'z'],  # z[x+10] = 100.12;
                        ['[', '['],
                        [:IDENTIFIER, 'x'],
                        ['+', '+'],
                        [:INT_NUM, '10'],
                        [']', ']'],
                        ['=', '='],
                        [:REAL_NUM, '100.12'],
                        [';', ';'],
                        ['}', '}'],
                        [false, false]
                       ]
# puts "#{t.prettyprint}"

# The parser can also parse from a file (stdin, actually).
tt = parser.parse_file
tt.prettyprint STDOUT

# prettyprint doesn't print a newline at the end; so, call puts once more
puts
