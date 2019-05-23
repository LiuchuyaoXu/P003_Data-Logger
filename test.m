clear all
clc
ardu=serial('COM3','Baudrate',9600);        %define ardu
fopen(ardu);            %open serial port
k1=fscanf(ardu);        %throw away the first sample

%initialise first 2 readings in y and first 2 values for moving average
k2=fscanf(ardu);
y(1)=str2num(k2);
current_avg(1)=y(1);
k3=fscanf(ardu);
y(2)=str2num(k3);
current_avg(2)=(y(2)+y(1))/2;

times_called=0; blink_between=0; check_duration=40; writedata='1';
disp('start testing now');
i=0;
while true
    i=i+1;
    if blink_between > 150
        fwrite(ardu,writedata,'uchar'); %write data
    end
    blink_between=blink_between+1;      %count how long you havent blinked
    k=fscanf(ardu);     
    y(i+1)=str2num(k);
    if current_avg(i)-y(i+1) > 8        %set threshold for how much reading has to have fallen before you check whether it is a blink
        times_called=times_called+1;        %count number of times we check
        prev_readings=[y(i) y(i+1)];
        [blink(times_called),readings(times_called,:)]=checkblink(ardu,check_duration,prev_readings,blink_between);
        if blink(times_called)==1
            disp('you blinked');
            disp(blink_between);
            blink_between=0;
        elseif blink(times_called)==0
            disp('not a blink');
            blink_between=blink_between+check_duration;
            disp(blink_between);
        end
    end
    current_avg(i+1)=movingavg(y, 15);      %update moving average
end
%disp(times_called);
fclose(ardu);
delete(ardu);
clear ardu