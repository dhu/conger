clear all;
%%%%%%%%%%%%%
%load dec_pkt;
%l = dec_pkt;
%%%%%%%%%%%%%
load lbl-tcp-3.tcp;
l = lbl_tcp_3;
%%%%%%%%%%%%%
%load l;
%%%%%%%%%%%%%
l(:, 4) = l(:, 2)*5000+l(:, 3);
whos l
results = zeros(10, 4);

for k = 1:1
%%%%%%%%%%%%%    
%    w = k*4;
%%%%%%%%%%%%%    
    w = k;
%%%%%%%%%%%%%    
   
    n = floor(600/w);
    counts = zeros(n, 4);
    for i = 1:n
        temp = 100*i/n;
        if (floor(temp/5) == temp/5)
            {k ': ' temp '%'}
        end
        z = l(find(l(:, 1) >= (i-1)*w & l(:, 1) < i*w ), :);
        sources = hist(z(:, 2), min(z(:,2)):max(z(:,2)));
        sources = length(sources(sources > 0));
        destinations = hist(z(:, 3), min(z(:,3)):max(z(:,3)));
        destinations = length(destinations(destinations > 0));
        combinations = hist(z(:, 4), min(z(:,4)):max(z(:,4)));
        combinations = length(combinations(combinations > 0));
        counts(i, :) = [length(z(:,1)) sources destinations combinations];
    end
    if (k == 1) 
        stream_rate = counts(:, 1);
    end
    results(k, :) = mean(counts);
end
results
stream_rate
