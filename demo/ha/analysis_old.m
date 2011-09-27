clear all; close all;

%%%%%%%%%%%%%%%%%%%%
%ha_loads = c_costs;
%for i = 1:N
%   processing_load = processing_loads(i);
%   for j =  1:length(c_costs(1, :, 1))
%       ha_loads(2*i-1, j, 2) = c_costs(2*i-1, j, 2)/c_costs(2*i-1, j, 1)/processing_load/1000;
%       ha_loads(2*i, j, 2) = c_costs(2*i, j, 2)/c_costs(2*i, j, 1)/processing_load/1000;
%   end
%end

%figure(2);hold on;
%x_label = 'checkpoint interval (sec)';
%y_label = '[HA load]/[processing load]';
%graph(group_names, ha_loads, title, x_label, y_label, symbols, [1 3]);
%legend(group_names, 0);
%max_y = max(max(ha_loads(:, :, 2)));
%max_y = 1.5;
%axis([0 max(2000, max(max(ha_loads(:, :, 1)))) 0 max_y]);
%axis([0 4 0 max_y]);
%set(gca, 'Xtick', [250 500 1000 1500 4000]);
%plot([1 1], [0  max_y], 'k:');

%processing_load = load_logs({'archive/input4.1.cpu.log'}, 1, 2);
%p1 = mean(processing_load(1, 2))
%processing_load = load_logs({'archive/input4.2.cpu.log'}, 1, 2);
%p2 = mean(processing_load(1, 2))
%inv([50 2450; 100 2400])*[p1;p2]

N = 2;

%%%%%%%%%%%%%%%%%%%%%%%%
file_names = {
'archive/input1.c_cost.primary.125.log',
'archive/input1.c_cost.primary.125.log',
'archive/input1.c_cost.primary.250.log',
'archive/input1.c_cost.primary.500.log',
'archive/input1.c_cost.primary.1000.log',
'archive/input1.c_cost.primary.1500.log',
'archive/input1.c_cost.primary.2000.log'
'archive/input1.c_cost.primary.3000.log'
'archive/input1.c_cost.primary.4000.log'
};
input2_p = load_logs(file_names, 2, 3);

file_names = {
'archive/input1.c_cost2.primary.125.log',
'archive/input1.c_cost2.primary.125.log',
'archive/input1.c_cost2.primary.250.log',
'archive/input1.c_cost2.primary.500.log',
'archive/input1.c_cost2.primary.1000.log',
'archive/input1.c_cost2.primary.1500.log',
'archive/input1.c_cost2.primary.2000.log'
'archive/input1.c_cost2.primary.3000.log'
'archive/input1.c_cost2.primary.4000.log'
};
input2_p2 = load_logs(file_names, 2, 3);
delta = input2_p2 - input2_p;
delta = 0.5*(delta + mean(delta(:, 2)));
delta = 0;
input2_p = input2_p + delta;

file_names = {
'archive/input1.c_cost.backup.125.log',
'archive/input1.c_cost.backup.125.log',
'archive/input1.c_cost.backup.250.log',
'archive/input1.c_cost.backup.500.log',
'archive/input1.c_cost.backup.1000.log',
'archive/input1.c_cost.backup.1500.log',
'archive/input1.c_cost.backup.2000.log'
'archive/input1.c_cost.backup.3000.log'
'archive/input1.c_cost.backup.4000.log'
};
input2_b = load_logs(file_names, 2, 3);
input2_b = input2_b  + delta;

processing_load = load_logs({'archive/input1.cpu.log'}, 1, 2);
processing_load_input2 = mean(processing_load(1, 2))

%%%%%%%%%%%%%%%%%%%
file_names = {
'archive/input2.c_cost.primary.125.log',
'archive/input2.c_cost.primary.125.log',
'archive/input2.c_cost.primary.250.log',
'archive/input2.c_cost.primary.500.log',
'archive/input2.c_cost.primary.1000.log',
'archive/input2.c_cost.primary.1500.log',
'archive/input2.c_cost.primary.2000.log'
'archive/input2.c_cost.primary.3000.log'
'archive/input2.c_cost.primary.4000.log'
};
input1_p = load_logs(file_names, 2, 3);

file_names = {
'archive/input2.c_cost2.primary.125.log',
'archive/input2.c_cost2.primary.125.log',
'archive/input2.c_cost2.primary.250.log',
'archive/input2.c_cost2.primary.500.log',
'archive/input2.c_cost2.primary.1000.log',
'archive/input2.c_cost2.primary.1500.log',
'archive/input2.c_cost2.primary.2000.log'
'archive/input2.c_cost2.primary.3000.log'
'archive/input2.c_cost2.primary.4000.log'
};
input1_p2 = load_logs(file_names, 2, 3);
delta = input1_p2 - input1_p;
delta = 0.5*(delta + mean(delta(:, 2)));
%delta = 0;
input1_p = input1_p + delta;

file_names = {
'archive/input2.c_cost.backup.125.log',
'archive/input2.c_cost.backup.125.log',
'archive/input2.c_cost.backup.250.log',
'archive/input2.c_cost.backup.500.log',
'archive/input2.c_cost.backup.1000.log',
'archive/input2.c_cost.backup.1500.log',
'archive/input2.c_cost.backup.2000.log'
'archive/input2.c_cost.backup.3000.log'
'archive/input2.c_cost.backup.4000.log'
};
input1_b = load_logs(file_names, 2, 3);
input1_b = input1_b  + delta;

processing_load = load_logs({'archive/input2.cpu.log'}, 1, 2);
processing_load_input1 = mean(processing_load(1, 2))

%%%%%%%%%%%%%%%%%%%

c_costs = zeros(2*N+1, length(input1_p(:, 1)), 2);
c_costs(1, :, :) = input1_p;


c_costs(2, :, :) = input1_p;
%c_costs(2, 1, 1) = 0.5*c_costs(2, 1, 1);

A = 0.39;
B = 3.46;

c = c_costs(2, :, 1);
c = [c; c_costs(2, :, 2)];
c = transpose(c)

a = 1;
w = 10;
l = 100;
s = 0*c(:, 1);
d = s;

sn = .93*1.2*13.8*10^-6
su = .94*1.8*5.6*10^-6

c(:, 1) = c(:, 1)/1000;

for i = 1:length(c(:, 1))
    I = c(i, 1)
    n_grs = (4*I)^A * exp(B)
    n_w_n = I/a*n_grs
    n_w_u = (w-I)*n_grs/a
    cc = sn*n_w_n+su*n_w_u
    c_costs(2, i, 2) = cc*1000;
end
c_costs(2, :, :)
c_costs(3, :, :) = input1_b;
c_costs(3, :, :)
c_costs(4, :, :) = input2_p;
c_costs(5, :, :) = input2_b;

processing_loads = zeros(N, 1);
processing_loads(1) = processing_load_input1;
processing_loads(2) = processing_load_input2;

%%%%%%%%%%%%%%%%%%%
figure(1); hold on;
group_names = {
'aggr(input 1, primary)'
'approximated'
'aggr(input 1, backup)'
'aggr(input 2, primary)'
'aggr(input 2, backup)'
};
symbols = {'k-', 'k:', 'k--', 'k-', 'k--'};
title = '';
x_label = 'checkpoint interval (sec)';
y_label = 'checkpoint cost (ms)';
c_costs(:, :, 1) = c_costs(:, :, 1)/1000;
graph(group_names, c_costs, title, x_label, y_label, symbols, [1 2 3]);
legend(group_names, 0);
max_y = max(max(c_costs(:, :, 2)));
max_y = 15;
%max_y = 20;
%axis([0 max(max(c_costs(:, :, 1))) 0 max_y]);
axis([0 4 0 max_y]);
%set(gca, 'Xtick', [250 500 1000 1500 4000]);
plot([1 1], [0  max_y], 'k:');

plot(c(:, 1), 1000*s, ':');

hold off;

