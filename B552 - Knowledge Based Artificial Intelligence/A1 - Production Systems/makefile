JFLAGS = -g
JC = javac
.SUFFIXES: .java .class
.java.class:
	$(JC) $(JFLAGS) $*.java

CLASSES = \
        rules.java \
        run_ps.java \
	wm.java

default: all

all: classes jar

classes: $(CLASSES:.java=.class)

clean:
	$(RM) *.class
	$(RM) *.jar
