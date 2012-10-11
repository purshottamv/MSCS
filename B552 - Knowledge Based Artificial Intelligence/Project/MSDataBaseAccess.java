package com;

import java.sql.Connection;
import java.sql.DriverManager;


public class MSDataBaseAccess {

	private static MSDataBaseAccess msDataBase;
	private String dbLocation = "jdbc:sqlserver://localhost:1433";
	private String userId = "sa"; // Provide the DataBase User name
	private String password = "IndianaUniversity+_)("; // Provide the DataBase Password 
	
	private MSDataBaseAccess() {
		// TODO Auto-generated constructor stub
	}
	
	public static MSDataBaseAccess getInstance()
	{
		if(msDataBase == null)
			return msDataBase = new MSDataBaseAccess();
		
		return msDataBase;
	}
	
	/**
	 * Returns a Connection Object to the Caller
	 * @return
	 */
	public Connection getConnection()
	{
		try {
	         Class.forName("com.microsoft.sqlserver.jdbc.SQLServerDriver");
	         return DriverManager.getConnection(dbLocation, userId, password);
	         
	      } catch (Exception e) {
	         return null;
	      }
	}
}
