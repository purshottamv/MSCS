
import java.awt.BorderLayout;
import java.awt.Image;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.image.ImageObserver;
import java.awt.image.ImageProducer;
import java.util.Random;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.ValueAxis;
import org.jfree.chart.plot.XYPlot;
import org.jfree.data.time.Millisecond;
import org.jfree.data.time.RegularTimePeriod;
import org.jfree.data.time.TimeSeries;
import org.jfree.data.time.TimeSeriesCollection;
import org.jfree.data.xy.XYDataset;
import org.jfree.ui.ApplicationFrame;
import org.jfree.ui.RefineryUtilities;

public class MonitorGui extends ApplicationFrame
{

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private final static String WINDOW_TITLE = "Resource Monitoring System";
	private final static String AGGREGATED_CPU_USAGE_TITLE = "Average CPU Usage";
	private final static String AGGREGATED_MEM_USAGE_TITLE = "Average MEMORY Usage";
	private final static String AGGREGATED_MPI_PROCESS_CPU_USAGE_TITLE = "Average MPI Process CPU Usage";
	private final static String AGGREGATED_MPI_PROCESS_MEM_USAGE_TITLE = "Average MPI Process MEMORY Usage";
	private final static double allCpuRange = 100/*1*//*1000*/;
	private final static double allMemRange = 100/*1000*/;
	private final static double mpiCpuRange = 100;
	private final static double mpiMemRange = 100;
	private static int processHold = 0;
	private static enum differentiator {ALL_CPU, ALL_MEM, MPI_CPU, MPI_MEM};
	
	private TimeSeries allCpuDisplay;
    private TimeSeries allMemDisplay;
    
    private TimeSeries mpiCpuDisplay;
    private TimeSeries mpiMemDisplay;
    
    // Main Panels
    private JPanel upperPanel;
	private JPanel lowerPanel;
    
    private JPanel masterPanel;
    private JPanel aggregatedProcessPanel;
    private JPanel mpiProcessPanel;
    
    private JScrollPane jScrollPanel;
    
    private JLabel noOfProc;
    
	public MonitorGui() {
		super(WINDOW_TITLE);
		// TODO Auto-generated constructor stub
		
		upperPanel = new JPanel();
		lowerPanel = new JPanel();
		
		masterPanel = new JPanel();
		aggregatedProcessPanel = new JPanel();
		mpiProcessPanel = new JPanel();
		
		allCpuDisplay = new TimeSeries(AGGREGATED_CPU_USAGE_TITLE, Millisecond.class);
		allMemDisplay = new TimeSeries(AGGREGATED_MEM_USAGE_TITLE, Millisecond.class);
		
		mpiCpuDisplay = new TimeSeries(AGGREGATED_MPI_PROCESS_CPU_USAGE_TITLE, Millisecond.class);
		mpiMemDisplay = new TimeSeries(AGGREGATED_MPI_PROCESS_MEM_USAGE_TITLE, Millisecond.class);
		
		ChartPanel allCpuChartPanel = new ChartPanel(getChart(new TimeSeriesCollection(allCpuDisplay), differentiator.ALL_CPU.toString()));
		allCpuChartPanel.setPreferredSize(new java.awt.Dimension(650, 350));
		aggregatedProcessPanel.add(allCpuChartPanel, BorderLayout.NORTH);
		
		ChartPanel allMemChartPanel = new ChartPanel(getChart(new TimeSeriesCollection(allMemDisplay), differentiator.ALL_MEM.toString()));
		allMemChartPanel.setPreferredSize(new java.awt.Dimension(650, 350));
		aggregatedProcessPanel.add(allMemChartPanel, BorderLayout.SOUTH);
		
		ChartPanel mpiCpuChartPanel = new ChartPanel(getChart(new TimeSeriesCollection(mpiCpuDisplay), differentiator.MPI_CPU.toString()));
		mpiCpuChartPanel.setPreferredSize(new java.awt.Dimension(650, 350));
		mpiProcessPanel.add(mpiCpuChartPanel, BorderLayout.NORTH);
		
		ChartPanel mpiMemChartPanel = new ChartPanel(getChart(new TimeSeriesCollection(mpiMemDisplay), differentiator.MPI_MEM.toString()));
		mpiMemChartPanel.setPreferredSize(new java.awt.Dimension(650, 350));
		mpiProcessPanel.add(mpiMemChartPanel, BorderLayout.SOUTH);

		JPanel labelPanel = new JPanel();
		JLabel processDefLabel = new JLabel();
		processDefLabel.setText("No.Of MPI Processes");
		noOfProc = new JLabel();
		noOfProc.setText("0");
		
		JLabel spaceLabel = new JLabel();
		spaceLabel.setText("");
		
		labelPanel.add(processDefLabel, BorderLayout.NORTH);
		labelPanel.add(noOfProc, BorderLayout.LINE_END);
		
		lowerPanel.add(labelPanel, BorderLayout.NORTH);
		lowerPanel.add(mpiProcessPanel, BorderLayout.SOUTH);
		lowerPanel.setLayout(new BoxLayout(lowerPanel, BoxLayout.Y_AXIS));
		
		upperPanel.add(spaceLabel, BorderLayout.NORTH);
		upperPanel.add(aggregatedProcessPanel, BorderLayout.SOUTH);
			
		//masterPanel.add(aggregatedProcessPanel, BorderLayout.NORTH);
		//masterPanel.add(mpiProcessPanel, BorderLayout.SOUTH);
		
		masterPanel.add(upperPanel, BorderLayout.NORTH);
		masterPanel.add(lowerPanel, BorderLayout.SOUTH);
		
		masterPanel.setPreferredSize(new java.awt.Dimension(1400, 800));

		jScrollPanel = new JScrollPane(masterPanel);
		jScrollPanel.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		jScrollPanel.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
		jScrollPanel.setPreferredSize(new java.awt.Dimension(1400, 800));
		
		setContentPane(jScrollPanel);
		//setIconImage(new Image)
	}
	
	private static JFreeChart getChart(final XYDataset dataset, String type) 
	{
		JFreeChart chart = null;
		if(differentiator.ALL_CPU == differentiator.valueOf(type))
			chart = ChartFactory.createTimeSeriesChart("AVG ALL PROCESS CPU UTILIZATION", "Time", "Percentage", dataset, true, true, false);
		else if(differentiator.ALL_MEM == differentiator.valueOf(type))
			chart = ChartFactory.createTimeSeriesChart("AVG ALL PROCESS MEM UTILIZATION", "Time", "Percentage", dataset, true, true, false);
		else if(differentiator.MPI_CPU == differentiator.valueOf(type))
			chart = ChartFactory.createTimeSeriesChart("AVG MPI PROCESS CPU UTILIZATION", "Time", "Percentage", dataset, true, true, false);
		else if(differentiator.MPI_MEM == differentiator.valueOf(type))
			chart = ChartFactory.createTimeSeriesChart("AVG MPI PROCESS MEM UTILIZATION", "Time", "Percentage", dataset, true, true, false);
        final XYPlot xyPlot = chart.getXYPlot();
        ValueAxis axis = xyPlot.getDomainAxis();
        axis.setAutoRange(true);
        axis.setFixedAutoRange(120000.0);
        axis = xyPlot.getRangeAxis();
        if(differentiator.ALL_CPU == differentiator.valueOf(type))
        	axis.setRange(0.0, allCpuRange);
        else if(differentiator.ALL_MEM == differentiator.valueOf(type))
        	axis.setRange(0.0, allMemRange);
        else if(differentiator.MPI_CPU == differentiator.valueOf(type))
        	axis.setRange(0.0, mpiCpuRange);
        else if(differentiator.MPI_MEM == differentiator.valueOf(type))
        	axis.setRange(0.0, mpiMemRange);
        return chart;
    }
	
	public void updateChart(double allCpuUpdateValue, double allMemUpdateValue, double mpiCpuUpdateValue, double mpiMemUpdateValue, int noOfMPIProcesses)
	{
    	//System.out.println("cpuUpdateValue -> "+cpuUpdateValue+", memUpdateValue ->"+memUpdateValue);
		allCpuDisplay.addOrUpdate(new Millisecond(), allCpuUpdateValue);
		allMemDisplay.addOrUpdate(new Millisecond(), allMemUpdateValue);
		
		mpiCpuDisplay.addOrUpdate(new Millisecond(), mpiCpuUpdateValue);
		mpiMemDisplay.addOrUpdate(new Millisecond(), mpiMemUpdateValue);
		
		if(noOfMPIProcesses>processHold)
		{
			processHold = noOfMPIProcesses;
			noOfProc.setText(""+processHold);
		}
    }
}
