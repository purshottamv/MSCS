#!/bin/sh
rm ./classes/*.class
javac -classpath ./lib/activemq-all-5.4.3.jar:./lib/jcommon-1.0.17.jar:./lib/jfreechart-1.0.14.jar ./src/MessageTypes.java
javac -classpath ./lib/activemq-all-5.4.3.jar:./lib/jcommon-1.0.17.jar:./lib/jfreechart-1.0.14.jar:./src/. ./src/MonitorGui.java
javac -classpath ./lib/activemq-all-5.4.3.jar:./lib/jcommon-1.0.17.jar:./lib/jfreechart-1.0.14.jar:./src/. ./src/Monitor.java
cp ./src/*.class ./classes
rm ./src/*.class
cd classes
java -classpath ../lib/activemq-all-5.4.3.jar:../lib/jcommon-1.0.17.jar:../lib/jfreechart-1.0.14.jar:. Monitor
