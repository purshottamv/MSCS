#!/bin/sh
rm -r SalesAgent.war  
cd WebContent/WEB-INF/
rm -r classes
cd ../..
cd build/classes/
rm -r *
cd ../..
javac -classpath ./lib/servlet-api.jar:./lib/sqljdbc4.jar:./src/com/. ./src/com/AttrImpPair.java
javac -classpath ./lib/servlet-api.jar:./lib/sqljdbc4.jar:./src/com/. ./src/com/AttrRangePair.java
javac -classpath ./lib/servlet-api.jar:./lib/sqljdbc4.jar:./src/com/. ./src/com/Product.java
javac -classpath ./lib/servlet-api.jar:./lib/sqljdbc4.jar:./src/com/. ./src/com/MSDataBaseAccess.java
javac -classpath ./lib/servlet-api.jar:./lib/sqljdbc4.jar:./src/com/. ./src/com/Rule.java
cd src/com
javac -classpath ../../lib/servlet-api.jar:../../lib/sqljdbc4.jar:../. ./Results.java
javac -classpath ../../lib/servlet-api.jar:../../lib/sqljdbc4.jar:../. ./DataAccess.java
javac -classpath ../../lib/servlet-api.jar:../../lib/sqljdbc4.jar:../. ./CaseEngine.java
javac -classpath ../../lib/servlet-api.jar:../../lib/sqljdbc4.jar:../. ./RuleEngine.java
javac -classpath ../../lib/servlet-api.jar:../../lib/sqljdbc4.jar:../. ./ControllerServlet.java
cd ../../build/classes
mkdir com
cd ../../src/com/
cp *.class ../../build/classes/com/
rm -r *.class
cd ../../build/
cp -r * ../WebContent/WEB-INF
cd ../WebContent
jar -cvf SalesAgent.war *
cp -r SalesAgent.war ../
rm -r SalesAgent.war 


