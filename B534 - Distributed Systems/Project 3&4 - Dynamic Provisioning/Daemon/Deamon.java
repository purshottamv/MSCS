import java.net.InetAddress;
import java.net.UnknownHostException;

import javax.jms.Connection;
import javax.jms.DeliveryMode;
import javax.jms.Destination;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageConsumer;
import javax.jms.MessageListener;
import javax.jms.MessageProducer;
import javax.jms.ObjectMessage;
import javax.jms.Session;

import org.apache.activemq.ActiveMQConnectionFactory;
import org.hyperic.sigar.Sigar;
import org.hyperic.sigar.ptql.ProcessFinder;


public class Deamon {
	
	private String deamonId;
	private final String brokerAddress = "tcp://129.79.49.181:61616";
	private final String dataQueue = "G26_Data_Queue";
	private final String syncTopic = "G26_Sync_Topic";
	private PublishDataThread publishDataThread;
	private SyncDeamonThread syncDeamonThread;
	private boolean registered;
	private Integer messgId;
	private int monitorState;
	
	/**
	 * 
	 * @param id
	 */
	private Deamon(String id)
	{
		this.deamonId = id;
		this.registered = false;
		this.messgId = new Integer(1);
		
		// Instantiate PublishDataThread
		publishDataThread = new PublishDataThread();
		syncDeamonThread = new SyncDeamonThread();
		publishDataThread.start();
		syncDeamonThread.startListening();
	}
	
	private class PublishDataThread extends Thread
	{
        private ActiveMQConnectionFactory connectionFactory;
        private Connection connection;
        private Session session;
        private Destination destination;
        private MessageProducer producer;
        private Sigar sigar;
        private ProcessFinder processFinder;
        private double totalRam;
        private boolean publishDataThread;
        private boolean registeredMessageSend;
        private int noOfCores;
        
        private final String uniqueProcessType = "mpi_main";
        
        public PublishDataThread ()
        {
        	try
        	{
	        	// Create a ConnectionFactory
	        	connectionFactory = new ActiveMQConnectionFactory(brokerAddress);
	        	
	        	// Create a Connection
	        	connection = connectionFactory.createConnection();
	        	connection.start();
	        	
	        	// Create a Session
	        	session = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
	        	
	        	// Create the destination (Topic or Queue)
                destination = session.createQueue(dataQueue);

                // Create a MessageProducer from the Session to the Topic or Queue
                producer = session.createProducer(destination);
                producer.setDeliveryMode(DeliveryMode.NON_PERSISTENT);
                
                // Initializing the Sigar Instance
                sigar = new Sigar();
                
                // Initiating the Process Finder Instance
                processFinder = new ProcessFinder(sigar);
                
                // Get the Total available RAM
                totalRam = sigar.getMem().getRam()*1024*1024*8;
                
                // Get Total Number Of Available Cores
                noOfCores = Runtime.getRuntime().availableProcessors();
                
                publishDataThread = true;
                registeredMessageSend = false;
        	}catch (Exception e) {
				// TODO: handle exception
        		System.out.println(" ========= Failed To Create Publish Data Thread ... Shutting Down The Deamon ========= ");
        		System.exit(0);
			}
        }
        
        @Override
        public void run() {
        	// TODO Auto-generated method stub
        	double allCpuStatTmp = 0;
        	double allCpuStat = 0;
        	double allMemStat = 0;
        	double mpiCpuStat = 0;
        	double mpiMemStat = 0;
        	
        	while(publishDataThread)
        	{
        		try
        		{
        			synchronized (messgId) {
					
        				if(messgId == 999999999)
            				messgId = 1;
            			
    	        		if(!registered && !registeredMessageSend)
    	        		{
    	        			producer.send(session.createObjectMessage(new MessageTypes.Register(deamonId)));
    	        			//System.out.println(" === Deamon Sending The Registration Message === ");
    	        			registeredMessageSend = true;
    	        		}else if(registered)
    	        		{
    	        			// Reset
    	        			mpiMemStat = 0;
    	        			mpiCpuStat = 0;
    	        			
    	        			// All Processes
    	        			allCpuStatTmp = sigar.getCpuPerc().getCombined();
    	        			allMemStat = sigar.getMem().getUsedPercent();
    	        			if(!Double.isNaN(allCpuStatTmp))
    	        				allCpuStat = allCpuStatTmp;
    	        			
    	        			// MPI Processes
    	        			long[] pids = processFinder.find("State.Name.eq="+uniqueProcessType);
    	        			
    	        			for(long eachPid:pids)
    	        	        {
    	        				//System.out.println(eachPid);
    	        				mpiCpuStat += sigar.getProcCpu(eachPid).getPercent()*100.0d;
    	        				mpiMemStat += (100/totalRam)*sigar.getProcMem(eachPid).getResident();
    	        	        }
    	        			
    	        			//System.out.println("CPU Usage -> "+cpuStat);
    	        			producer.send(session.createObjectMessage(new MessageTypes.Data(deamonId, allMemStat, (allCpuStat*100), mpiMemStat, (mpiCpuStat/noOfCores), 
    	        					messgId, monitorState, pids.length)));
    	        			
    	        			messgId++;
    	        			Thread.sleep(5);
    	        		}
					}
        				
        		}catch(Exception e)
        		{
        			e.printStackTrace();
        		}
        	}
        	
        	// Clean up
            try {
				session.close();
				connection.close();
			} catch (JMSException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
        }
	}
	
	private class SyncDeamonThread implements MessageListener
	{
		
		ActiveMQConnectionFactory connectionFactory;
        Connection connection;
        Session session;
        Destination destination;
        MessageConsumer consumer;
        
        public SyncDeamonThread()
        {
        	try{
	        	// Create a ConnectionFactory
	        	connectionFactory = new ActiveMQConnectionFactory(brokerAddress);
	        	
	        	// Create a Connection
	        	connection = connectionFactory.createConnection();
	        	connection.start();
	        	
	        	// Create a Session
	        	session = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
	        	
	        	// Create the destination (Topic or Queue)
	        	destination = session.createTopic(syncTopic);
	        	
	        	// Create a MessageConsumer from the Session to the Topic or Queue
	        	consumer = session.createConsumer(destination);
	        	
        	}catch(Exception e)
        	{
        		
        	}
        }
        
        public void startListening()
        {
        	// Set Message Listener
        	try {
				consumer.setMessageListener(this);
			} catch (JMSException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
        	
        }

		@Override
		public void onMessage(Message message) {
			// TODO Auto-generated method stub
			try {
				Object obj = ((ObjectMessage)message).getObject();
				if(obj instanceof MessageTypes.Sync)
				{
					synchronized (messgId) {
					
						messgId = 1;
						monitorState = ((MessageTypes.Sync)obj).getMonitorState();
						registered = true;
						System.out.println(" ==== Deamon Is Synchronized ==== ");
					}
				}
			} catch (JMSException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		try {
			new Deamon(InetAddress.getLocalHost().getHostName()+"_"+InetAddress.getLocalHost().getHostAddress());
		} catch (UnknownHostException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			System.out.println(" ==== Failed To Get Host Name ==== Shutting Down The Deamon ==== ");
			System.exit(0);
		}
		
	}
}
