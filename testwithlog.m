clear all
clc
ardu=serial('COM3','Baudrate',9600);        %define ardu
fopen(ardu);            %open serial port
k1=fscanf(ardu);        %throw away the first sample

%initialise first 2 readings in y and first 2 values for moving average
k2=fscanf(ardu);
y(1)=str2double(k2);
current_avg(1)=y(1);
k3=fscanf(ardu);
y(2)=str2double(k3);
current_avg(2)=(y(2)+y(1))/2;

times_called=0; blink_between=0; check_duration=35; writedata='1';
i=0;

figure
grid
h = animatedline;
ax = gca;
ax.YLim = [0 1024];
startTime = datetime('now');

disp('start testing now');
while true
    i=i+1;      %update index
    
    if blink_between > 400      %sound buzzer if you havent blinked for some time
        %fwrite(ardu,writedata,'uchar'); 
    end
    
    blink_between=blink_between+1;      %count how long you havent blinked
    
    k=fscanf(ardu);     %collect data from arduino
    y(i+1)=str2double(k);
    
    t =  datetime('now')-startTime;
    addpoints(h,datenum(t),y(i+1))      %add points to animation
    ax.XLim = datenum([t-seconds(15) t]);       %update axes
    datetick('x','keeplimits')          
    drawnow limitrate     %consider drawnow limitrate
    
    if current_avg(i)-y(i+1) > 8        %set threshold for how much reading has to have fallen before you check whether it is a blink
        prev_readings=[y(i) y(i+1)];
        [blink,readings]=checkblink(ardu,check_duration,prev_readings,blink_between,h,ax,startTime);
        y=[y readings(3:end)];      %append readings obtained during check to y vector
        i=i+check_duration;     %update index
        if blink==1
            disp('you blinked');
            disp(blink_between);        %display time since last blink
            blink_between=0;        %reset time between blinks to zero
        elseif blink==0
            disp('not a blink');
            blink_between=blink_between+check_duration;     %if not a blink, addd the checking time to time you havent blinked
            disp(blink_between);
        end
    end
    
    current_avg(i+1)=movingavg(y,15);      %update moving average
end

fclose(ardu);
delete(ardu);
clear ardu