import java.io.Serializable;


public class MessageTypes {

	public static class Register implements Serializable
	{
		private String deamonId;
		
		public Register(String Id)
		{
			this.deamonId = Id;
		}
		
		public String getDeamonId() 
		{
			return deamonId;
		}
	}
	
	public static class Sync implements Serializable
	{
		private int monitorState;
		
		public Sync(int monitorState)
		{
			this.monitorState = monitorState;
		}
		
		public int getMonitorState() {
			return monitorState;
		}
	}
	
	public static class Data implements Serializable
	{
		private String deamonId;
		private double allProcessMemConsp;
		private double allProcessCpuConsp;
		private double mpiProcessMemConsp;
		private double mpiProcessCpuConsp;
		private int mssgId;
		private boolean used;
		private int monitorState;
		private int noOfMPIProcesses;
		
		public Data(String deamonId, double allProcessMemConsp, double allProcessCpuConsp, double mpiProcessMemConsp, double mpiProcessCpuConsp, int messgId, int monitorState, int noOfMPIProcesses)
		{
			this.deamonId = deamonId;
			this.allProcessMemConsp = allProcessMemConsp;
			this.allProcessCpuConsp = allProcessCpuConsp;
			this.mpiProcessMemConsp = mpiProcessMemConsp;
			this.mpiProcessCpuConsp = mpiProcessCpuConsp;
			this.mssgId = messgId;
			this.used = false;
			this.monitorState = monitorState;
			this.noOfMPIProcesses = noOfMPIProcesses;
		}
		
		public int getNoOfMPIProcesses() {
			return noOfMPIProcesses;
		}
		
		public int getMonitorState() {
			return monitorState;
		}
		
		public String getDeamonId() {
			return deamonId;
		}

		public double getAllProcessMemConsp() {
			return allProcessMemConsp;
		}

		public double getAllProcessCpuConsp() {
			return allProcessCpuConsp;
		}
		
		public double getMpiProcessMemConsp() {
			return mpiProcessMemConsp;
		}

		public double getMpiProcessCpuConsp() {
			return mpiProcessCpuConsp;
		}
		
		public int getMssgId() {
			return mssgId;
		}
		
		public boolean isUsed()
		{
			return used;
		}
		
		public void markUsed()
		{
			used = true;
		}

	}
}
