JFLAGS = -g
JC = javac

.SUFFIXES: .java .class

.java.class:
	$(JC) $(JFLAGS) $*.java

CLASSES = \
		  QuickSort/QuickSortSimple.java \
		  QuickSort/QuickSortThread.java \
		  Problema2.java \

default: classes

classes: $(CLASSES:.java=.class)

clean:
	$(RM) *.class
	
run:
	java Problema2