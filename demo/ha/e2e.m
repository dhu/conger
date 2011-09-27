clear all; close all;

load archive/output.0.log
o1 = [(output(:, 1)*100+output(:, 2)) output(:,4)];
o1 = o1(1:8000, :);

load archive/output.1.log
o2 = [(output(:, 1)*100+output(:, 2)) output(:,4)];
o2 = o2(1:8000, :);

load archive/output.2.log
o3 = [(output(:, 1)*100+output(:, 2)) output(:,4)];
o3 = o3(1:8000, :);

load archive/output..log
o4 = [(output(:, 1)*100+output(:, 2)) output(:,4)];
o4 = o4(1:8000, :);

E(1, :, :) = [o1(:, 1) o1(:,2)];
E(2, :, :) = [o2(:, 1) o2(:,2)];
E(3, :, :) = [o3(:, 1) o3(:,2)];
E(4, :, :) = [o4(:, 1) o4(:,2)];

J = E(:, 3000:4000, :);
%J(:, :, 1) = J(:, :, 1) - min(min(J(:, :, 1)));
J(:, :, 1) = J(:, :, 1) * 10;
J = J * 0.001;

figure(1); hold on;
group_names = {
'whole checkpointing'
'round robin'
'min max'
'no checkpointing'
};
symbols = {'k-.', 'k:', 'k-', 'k-'};
title = '';
x_label = 'time (sec)';
y_label = 'end-to-end latency (sec)';
graph(group_names, J, title, x_label, y_label, symbols, [4]);
legend(group_names, 0);

min_x = ceil(min(min(J(:, :, 1))));
max_x = ceil(max(max(J(:, :, 1))));
max_y = ceil(max(max(J(:, :, 2))));
axis([min_x max_x 0 max_y]);
