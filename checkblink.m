function [y,readings]=checkblink(ardu,check_duration,prev_readings,blink_between)
readings=prev_readings;
%x=linspace(1,check_duration,check_duration);
for i=1:check_duration
    if blink_between>150
        fwrite(ardu,'1','uchar');
    end
    k=fscanf(ardu);
    readings(i+2)=str2num(k);
end

r_negated=-1*readings;

[pks,locs,w,p] = findpeaks(r_negated,'MinPeakProminence',30,'MinPeakDistance',10,'MinPeakWidth',3,'MaxPeakWidth',30);

if length(pks)==1
    y=1;
else
    y=0;
end

% rfilter = medfilt1(r,10,'truncate');
% changepoints=findchangepts(rfilter,'Statistic','linear','MinThreshold',200);
% 
% if length(changepoints)<3
%     y=0;
% elseif length(changepoints)==3
%     y=1;
% else
%     y=2;
% end

end