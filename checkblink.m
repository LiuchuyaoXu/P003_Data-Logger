function [y,readings]=checkblink(ardu,check_duration,prev_readings,blink_between,h,ax,startTime)
readings=prev_readings;
for i=1:check_duration
    if blink_between > 400
        %fwrite(ardu,'1','uchar');
    end
    k=fscanf(ardu);
    readings(i+2)=str2double(k);
    t =  datetime('now') - startTime;
    % Add points to animation
    addpoints(h,datenum(t),readings(i+2))
    % Update axes
    ax.XLim = datenum([t-seconds(15) t]);
    datetick('x','keeplimits')
    drawnow limitrate
end

r_negated=-1*readings;

[pks,locs,w,p] = findpeaks(r_negated,'MinPeakProminence',30,'MinPeakDistance',10,'MinPeakWidth',3,'MaxPeakWidth',30);

if length(pks)==1
    y=1;
else
    y=0;
end

end