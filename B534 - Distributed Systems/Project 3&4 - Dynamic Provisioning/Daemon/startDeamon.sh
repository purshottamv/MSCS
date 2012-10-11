#!/bin/sh
export JAVA_HOME=/N/u/taklwu/jdk1.6.0_31
export PATH=$JAVA_HOME/bin:$MPJ_HOME/bin:$PATH
export CLASSPATH=$CLASSPATH:$JAVA_HOME/lib
rm ./classes/*.class

javac -classpath ./lib/activemq-all-5.4.3.jar:./lib/sigar.jar ./src/MessageTypes.java

javac -classpath ./lib/activemq-all-5.4.3.jar:./lib/sigar.jar:./src/. ./src/Deamon.java
cp ./src/*.class ./classes
rm ./src/*.class
cd classes
java -classpath ../lib/activemq-all-5.4.3.jar:../lib/sigar.jar:. Deamon &
