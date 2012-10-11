%--------------------------------------------------------------------------------------
%
% Script to plot measurements of the overhead when calling a timer function in
% C/C++/Fortran. This is *not* a measure of clock resolution. Although the cost in
% time for other function calls probably are about the same, don't count on it. In
% particular long argument lists and complex data objects being passed might use
% completely different function call mechanisms.  But you knew that already, right?
%
% The data file must be named "data_overheads", and it needs to have one timing
% per row. Each row needs to have two numbers, the timing and then the number
% of flops that corresponds to that timing. 
% 
% The times and flop estimates are sorted independently for plotting because it
% makes a less busy-looking graph and the main goal is the average times/flops,
% not trends. Hey, you don't like it, then remove the sort()s below and have at it.
% 
% The plots show the data points (well, duhh) but also include lines connecting them
% to make the data points easier to spot.  Of course, there is no physical meaning
% to the values between two measurements. It's just for your eyeballs, don't get in
% a tizzy over it.
% 
% Usage: crank up Matlab in the directory with the data file, then type the 
% command "show_overheads" in the (surprise, surprise) command window.
% 
% Once you have the overhead and resolution, just make sure that any timing block is
% > 100*(overhead+resolution) in future timings.
%
%---------------
% Randall Bramley
% Department of Computer Science
% Indiana University, Bloomington
%-----------------
% Started: Fri 28 Sep 2007, 06:38 PM
% Modified: Mon 27 Aug 2012, 06:58 AM for p573
% Last Modified: Mon 27 Aug 2012, 07:09 AM 
%--------------------------------------------------------------------------------------

%--------------------------------------------------------------
% Set to true to eliminate bad values, false to NaN-ify them.  
%--------------------------------------------------------------
ditch_it_or_nan_it = true; 

%----------------------
% Create plots or not  
%----------------------
regularplot  = true;
semilogxplot = false;  % not as useful as it might seem
flopplot     = true;  
paintitblack = false;

overheads = load('data_overheads');
vlength   = size(overheads, 1);
ohtimings = sort(overheads(:, 1));
ohflops   = sort(overheads(:, 1));

%------------------------------
% Don't plot nonsense timings  
%------------------------------
I = find(ohtimings <= 0 | isnan(ohtimings));
if ditch_it_or_nan_it
    ohtimings = ohtimings(setdiff(1:vlength, I));
    ohflops   = ohflops(setdiff(1:vlength,   I));
else
    ohtimings(I) = NaN;
    ohflops(I)   = NaN;
end

vlength    = size(ohtimings); % Reset the vector lengths in case I ~= []   
floplength = size(ohflops, 1);

%-----------------------------------------------------------------
% This may look redundant but there are cases where the timing is 
% positive but the corresponding number of flops is 0.
%-----------------------------------------------------------------
J = find(ohflops <= 0 | isnan(ohflops));
if ditch_it_or_nan_it
    ohflops = ohflops(setdiff(1:floplength, J));
else
    ohflops(J) = NaN;
end
floplength = size(ohflops, 1); % Reset the flop vector length if J ~= []   

tttm = mean(ohtimings); % Get the means and std variations for the data
fffm = mean(ohflops);
ttts = std(ohtimings);
fffs = std(ohflops);

disp(sprintf('\nAverage overhead time = %g seconds', tttm));
disp(sprintf('Standard deviation for overhead times = %g seconds', ttts));

if flopplot
    disp(sprintf('\nAverage overhead flops = %g', fffm));
    disp(sprintf('Standard deviation for overhead flops = %g\n', fffs));
end

if paintitblack
    colordef black
    bstring = 'y+';
else
    bstring = 'b+';
end
if regularplot 
    figure;
    plot(1:vlength, ohtimings, bstring, ...
         1:vlength, ohtimings, 'r-');
    xlabel('Timing number')
    ylabel('Seconds')
    title('Sorted Overheads in Seconds for Calling Timing Function')
    grid on
end
    
if semilogxplot
    figure;
    semilogy(1:vlength, ohtimings, bstring, ...
             1:vlength, ohtimings, 'r-');
    xlabel('Timing number')
    ylabel('Seconds')
    title('Sorted Overheads in Seconds for Calling Timing Function (log scale)')
    grid on
end
    
if flopplot
    figure;
    plot(1:floplength, ohflops, bstring, ...
         1:floplength, ohflops, 'r-');
    xlabel('Timing number')
    ylabel('Number of flops')
    title('Sorted Overheads in Flops for Calling Timing Function')
    grid on
end

% If the figures were on dark backgrounds, reset it back to white
if paintitblack
    colordef white
end
    
