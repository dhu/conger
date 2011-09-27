clear all; close all;

load archive/6units.10.recovery_time.log
ert1 = X6units;
load archive/6units.11.recovery_time.log
ert2 = X6units;
load archive/6units.12.recovery_time.log
ert3 = X6units;
 
set(gca, 'FontSize', 18);

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

figure(1); hold on;
plot(ert1(:,1), ert1(:, 2), 'k--');
plot(ert2(:,1), ert2(:, 2), 'k:');
plot(ert3(:,1), ert3(:, 2), 'k-', 'markersize', 14, 'linewidth', 3);
hold off;
