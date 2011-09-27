clear all;
close all;

fontsize = 14;
linewidth = 1;

load server1.statistics.micro.dat;
[a1101c, a1101L] = sieve(server1, 1101);
[a1201c, a1201L] = sieve(server1, 1201);

a1101L
a1101L = a1101L*0.01; % cost for 1 sec replay
a1101L = [a1101c(:, 1) a1101L*1000*a1101c(:, 1)]; % scaled for millisec.

load server2.statistics.micro.dat;
a1101p = sieve(server2, 1101);
a1101c = a1101c(1:length(a1101p(:,1)), :);
a1101p(:, 3:4) = a1101c(:, 3:4);

a1201p = sieve(server2, 1201);
a1201c = a1201c(1:length(a1201p(:,1)), :);
a1201p(:, 3:4) = a1201c(:, 3:4);

a1101c = condense(a1101c);
a1101p = condense(a1101p);
a1201c = condense(a1201c);
a1201p = condense(a1201p);

c = clean([a1101c; a1201c]);

X1 = c(:, 3:4); y1 = c(:, 2);
[c1, bint1] = regress(y1, X1, 0.05);
c1
a1101ec = [a1101c(:,1) a1101c(:, 3:4)*c1];
a1201ec = [a1201c(:,1) a1201c(:, 3:4)*c1];
e1 = (X1*c1 - y1)./y1;
e1 = mean(abs(e1))

p = clean([a1101p; a1201p]);

X1 = p(:, 3:4); y1 = p(:, 2);
[p1, bint1] = regress(y1, X1, 0.05);
p1
a1101ep = [a1101p(:,1) a1101p(:, 3:4)*p1];
a1201ep = [a1201p(:,1) a1201p(:, 3:4)*p1];
e1 = (X1*p1 - y1)./y1;
e1 = mean(abs(e1))

%=========================
figure(1);hold on;
group_names = {
'aggr(input 1, capture)'
'aggr(input 1, estimated capture)'
'aggr(input 1, paste)'
'aggr(input 1, replay)'
'aggr(input 2, capture)'
'aggr(input 2, paste)'
};
set(gca, 'FontSize', fontsize);
set(gca, 'LineWidth', linewidth);

max_y = 90;

k = 1.5; % scale factor for better display
plot(a1101c(:, 1), k*a1101c(:, 2), 'k-', 'linewidth', 3); 
plot(a1101ec(:, 1), k*a1101ec(:, 2), 'g--', 'linewidth', 2); 
plot(a1101p(:, 1), k*a1101p(:, 2), 'k-');
plot(a1101L(:, 1), a1101L(:, 2), 'k:', 'linewidth', 3);
plot(a1201c(:, 1), a1201c(:, 2)/2, 'k-.', 'linewidth', 3);
plot(a1201p(:, 1), a1201p(:, 2)/2, 'k-.');

%plot(a1101ep(:, 1), k*a1101ep(:, 2), 'r:', 'linewidth', 2); 
%plot(a1201ec(:, 1), a1201ec(:, 2)/2, 'g:', 'linewidth', 2); 
%plot(a1201ep(:, 1), a1201ep(:, 2)/2, 'g:', 'linewidth', 2); 

plot([1 1], [0 max_y], 'k-.');
plot([10 10], [0 max_y], 'k-.');

legend(group_names, 'Location', 'NorthWest');
axis([0 13 0 max_y]);
set(gca, 'XTick', [0 1 2 4 6 8 10 12 14]);
ylabel('cost (msec)');
xlabel('checkpoint interval (sec)');

hold off;

