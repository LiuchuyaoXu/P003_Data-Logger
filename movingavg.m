function k=movingavg(x, num_points)
if length(x)<=num_points
    k=sum(x)/length(x);
else 
    lastelements=x(end-num_points+1:end);
    k=sum(lastelements)/num_points;
end