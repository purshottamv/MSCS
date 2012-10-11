import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
import java.util.Random;

import javax.jms.Connection;
import javax.jms.DeliveryMode;
import javax.jms.Destination;
import javax.jms.ExceptionListener;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageConsumer;
import javax.jms.MessageListener;
import javax.jms.MessageProducer;
import javax.jms.ObjectMessage;
import javax.jms.Session;

import org.apache.activemq.ActiveMQConnectionFactory;
import org.jfree.ui.RefineryUtilities;

public class Monitor {

	private final String brokerAddress = "tcp://129.79.49.181:61616";
	private final String dataQueue = "G26_Data_Queue";
	private final String syncTopic = "G26_Sync_Topic";
	private ConsumeDataThread consumeDataThread;
	private SyncDeamonThread syncDeamonThread;
	private DisplayThread displayThread;
	private HashMap<String, Queue<MessageTypes.Data>> dataBuffer;
	private Monitor monitor;
	private boolean sync;
	private int monitorState;
	private Random random;
	
	private Monitor()
	{
		dataBuffer = new HashMap<String, Queue<MessageTypes.Data>>();
		monitor = this;
		sync = false;
		
		random = new Random();
		consumeDataThread = new ConsumeDataThread();
		syncDeamonThread = new SyncDeamonThread();
		displayThread = new DisplayThread();
		consumeDataThread.startListening();
		syncDeamonThread.start();
		displayThread.start();
	}
	
	private class ConsumeDataThread implements ExceptionListener, MessageListener
	{
        ActiveMQConnectionFactory connectionFactory;
        Connection connection;
        Session session;
        Destination destination;
        MessageConsumer consumer;
        
        public ConsumeDataThread()
        {
        	try{
	        	// Create a ConnectionFactory
	        	connectionFactory = new ActiveMQConnectionFactory(brokerAddress);
	        	
	        	// Create a Connection
	        	connection = connectionFactory.createConnection();
	        	connection.start();
	        	connection.setExceptionListener(this);
	        	
	        	// Create a Session
	        	session = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
	        	
	        	// Create the destination (Topic or Queue)
	        	destination = session.createQueue(dataQueue);
	        	
	        	// Create a MessageConsumer from the Session to the Topic or Queue
	        	consumer = session.createConsumer(destination);
        	}catch(Exception e)
        	{
        		e.printStackTrace();
        		System.out.println("Connection Exception occured in Consumer Data Thread ... Shutting down the Monitor");
        		System.exit(0);
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
				System.exit(0);
			}
        }

		@Override
		public void onException(JMSException e) {
			// TODO Auto-generated method stub
			e.printStackTrace();
		}

		@Override
		public void onMessage(Message message) {
			try{
				// TODO Auto-generated method stub
				Object obj = ((ObjectMessage)message).getObject();
				
				if( obj instanceof MessageTypes.Register )
				{
					//System.out.println("Registration Message Received");
					dataBuffer.put(((MessageTypes.Register) obj).getDeamonId(), new LinkedList<MessageTypes.Data>());
					
					// Clear Buffer
					synchronized (monitor) {
						synchronized (dataBuffer) {
							Iterator<String> dataBufferKeySetIterator = dataBuffer.keySet().iterator();
							while(dataBufferKeySetIterator.hasNext())
							{
								dataBuffer.get(dataBufferKeySetIterator.next()).clear();
							}
							monitorState = random.nextInt();
						}
					}
					
					
					// Send Synchronize Message to all Deamons
					sync = true;
				}else if( obj instanceof MessageTypes.Data )
				{
					MessageTypes.Data data = (MessageTypes.Data) obj;
					if(data.getMonitorState() == monitorState)
					{
						dataBuffer.get(data.getDeamonId()).offer(data);
					}else
						System.out.println(" ======== Old Monitor Data ... Discarding ========= "+data.getDeamonId());
				}
			}catch(Exception e)
			{
				
			}
		}
	}
	
	private class SyncDeamonThread extends Thread
	{
		
		ActiveMQConnectionFactory connectionFactory;
        Connection connection;
        Session session;
        Destination destination;
        MessageProducer producer;
        
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
	        	producer = session.createProducer(destination);
	        	producer.setDeliveryMode(DeliveryMode.NON_PERSISTENT);
        	}catch(Exception e)
        	{
        		
        	}
        	
        	
        }
		
		@Override
		public void run() {
			// TODO Auto-generated method stub
			while(true)
			{
				try
				{
					if(sync)
					{
						//System.out.println("Sending SYNC Messages");
						producer.send(session.createObjectMessage(new MessageTypes.Sync(monitorState)));
						sync = false;
					}
					Thread.sleep(100);
				}catch (Exception e) {
					// TODO: handle exception
				}
			}
		}
	}
	
	private class DisplayThread extends Thread
	{
		private MonitorGui monitorGui;
		public DisplayThread ()
		{
			monitorGui = new MonitorGui();
			monitorGui.pack();
		    RefineryUtilities.centerFrameOnScreen(monitorGui);
		    monitorGui.setVisible(true);
		}
		
		private boolean bufferAutoCorrect()
		{
			Iterator<String> dataBufferKeySetIterator = dataBuffer.keySet().iterator();
			int mssgIdState = 0;
			boolean mssgIdStateHold = false;
			
			while(dataBufferKeySetIterator.hasNext())
			{
				MessageTypes.Data data = null;
				if((data = dataBuffer.get(dataBufferKeySetIterator.next()).peek()) != null)
				{
					if(!mssgIdStateHold)
					{
						mssgIdState = data.getMssgId();
						mssgIdStateHold = true;
					}
					
					if(data.getMssgId() != mssgIdState)
					{
						System.out.println(" ===== Synchronizing The Buffer ===== ");
						int maxMssgId;
						
						secondary_1:
						while(true)
						{
							Iterator<String> dfksIterator = dataBuffer.keySet().iterator();
							List<Integer> syncData = new ArrayList<Integer>();
							while(dfksIterator.hasNext())
							{
								MessageTypes.Data dt = null;
								if((dt = dataBuffer.get(dfksIterator.next()).peek()) != null)
								{
									syncData.add(dt.getMssgId());
								} else
								{
									try {
										Thread.sleep(100);
										continue secondary_1;
									} catch (InterruptedException e) {
										// TODO Auto-generated catch block
										e.printStackTrace();
									}
								}
							}
							
							Integer syncArray[] = syncData.toArray(new Integer[syncData.size()]);
							Arrays.sort(syncArray);
							maxMssgId = syncArray[syncData.size()-1];
							break secondary_1;
						}
						
						
						Iterator<String> dfksIterator = dataBuffer.keySet().iterator();
						secondary_2:
						while(dfksIterator.hasNext())
						{
							String tmpDeamonId = dfksIterator.next();
							while(true)
							{
								if( dataBuffer.get(tmpDeamonId).peek() != null && dataBuffer.get(tmpDeamonId).peek().getMssgId() != maxMssgId )
								{
									dataBuffer.get(tmpDeamonId).poll();
								}else if(dataBuffer.get(tmpDeamonId).peek().getMssgId() == maxMssgId)
								{
									continue secondary_2;
								}else if(dataBuffer.get(tmpDeamonId).peek() == null)
								{
									try {
										Thread.sleep(100);
									} catch (InterruptedException e) {
										// TODO Auto-generated catch block
										e.printStackTrace();
									}
								}
							}
						}
						
					}
				}else
				{
					return true;
				}
			}
			return false;
		}
		
		private void updateGui(List<MessageTypes.Data> newData)
		{
			//System.out.println("=== Inside updateGUI with size -> "+ newData.size());
			double allProcessCpuUpdateValue = 0, allProcessMemUpdateValue = 0, mpiProcessCpuUpdateValue = 0, mpiProcessMemUpdateValue = 0;
			int noOfMPIProcesses = 0;
			
			Iterator<MessageTypes.Data> newDataIterator = newData.iterator();
			
			while(newDataIterator.hasNext())
			{
				MessageTypes.Data tmpData = newDataIterator.next();
				/*if(Double.isNaN(tmpData.getCpu()))
					return; // Discarding the Series*/
				allProcessCpuUpdateValue += tmpData.getAllProcessCpuConsp();
				allProcessMemUpdateValue += tmpData.getAllProcessMemConsp();
				mpiProcessCpuUpdateValue += tmpData.getMpiProcessCpuConsp();
				mpiProcessMemUpdateValue += tmpData.getMpiProcessMemConsp();
				noOfMPIProcesses += tmpData.getNoOfMPIProcesses();
			}
			
			allProcessCpuUpdateValue = allProcessCpuUpdateValue/newData.size();
			allProcessMemUpdateValue = allProcessMemUpdateValue/newData.size();
			mpiProcessCpuUpdateValue = mpiProcessCpuUpdateValue/newData.size();
			mpiProcessMemUpdateValue = mpiProcessMemUpdateValue/newData.size();
			
			monitorGui.updateChart(allProcessCpuUpdateValue, allProcessMemUpdateValue, mpiProcessCpuUpdateValue, mpiProcessMemUpdateValue, noOfMPIProcesses);
		}
		
		@Override
		public void run() {
			// TODO Auto-generated method stub
			master:
			while(true)
			{
				try{
					// Perform Buffer Correction If Required
					if(bufferAutoCorrect())
						continue master;
					
					// Obtain The Synchronized Series Of Data
					Iterator<String> dataBufferKeySetIterator = dataBuffer.keySet().iterator();
					List<MessageTypes.Data> newData = new ArrayList<MessageTypes.Data>();
					while(dataBufferKeySetIterator.hasNext())
					{
						synchronized (dataBuffer) {
							MessageTypes.Data tmpData = dataBuffer.get(dataBufferKeySetIterator.next()).poll();
							if(tmpData == null)
							{
								continue master;
							}
							else
							{
								newData.add(tmpData);
							}
						}
						
					}
					
					// Update the GUI
					if(!newData.isEmpty())
					{
						updateGui(newData);
					}//else
					Thread.sleep(5);
				}catch(Exception e)
				{
					System.out.println(" === Buffered Cleared === ");
					continue master;
				}
			}
		}
	}
	
	public static void main(String args[])
	{
		new Monitor();
	}
}
