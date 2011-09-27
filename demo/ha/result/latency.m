close all; clear all;

start_time = 10000; % 10 sec
end_time = start_time+120000; % 70 sec

%=========================
load server1.recovery_time.200.dat

s01 = server1;

min_x = min(s01(:, 1));
s01(:, 1) = s01(:, 1) - min_x

%======================
load output.200.dat;
a01 = output(find(output(:, 1) == 1211), [3 4]);
a01(:, 2) = a01(:, 2) - a01(:, 1);
a01(:, 1) = a01(:, 1) - min_x;
%===================================

%====================================
a = s01;
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
s01 = a;
a = a01;
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
a01 = a;
%====================================



%=========================
load server1.recovery_time.201.dat
s11 = server1;

min_x = min(s11(:, 1));
s11(:, 1) = s11(:, 1) - min_x
%======================
load output.201.dat;
a11 = output(find(output(:, 1) == 1211), [3 4]);
a11(:, 2) = a11(:, 2) - a11(:, 1);
a11(:, 1) = a11(:, 1) - min_x;
%===================================

%====================================
a = s11;
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
s11 = a;
%====================================
a = a11;
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
a11 = a;
%====================================



%=========================
load server1.recovery_time.202.dat
s21 = server1;
min_x = min(s21(:, 1));
s21(:, 1) = s21(:, 1) - min_x;
%======================
load output.202.dat;
a21 = output(find(output(:, 1) == 1211), [3 4]);
a21(:, 2) = a21(:, 2) - a21(:, 1);
a21(:, 1) = a21(:, 1) - min_x;
%===================================
a = s21;
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
s21 = a;
%====================================
a = a21;
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
a21 = a;
%====================================
load output.dat;
a = output(find(output(:, 1) == 1211), [3 4]);
a(:, 2) = a(:, 2) - a(:, 1);
a(:, 1) = (a(:, 1) - min(a(:, 1)));
%===================================
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
%====================================

markersize = 12;
fontsize = 14;
linewidth = 1;

%====================================
figure(1);hold on;
set(gca, 'FontSize', fontsize);
set(gca, 'LineWidth', linewidth);
plot(s01(:, 1), s01(:, 2), 'k:');
plot(s11(:, 1), s11(:, 2), 'k--');
plot(s21(:, 1), s21(:, 2), 'k-');
xlabel('time (sec)'); 
ylabel('expected recovery time (sec)'); 
axis([60 80 0 4]);
legend({'Whole Checkpointing', 'Round Robin', 'Min Max'}, 0)

figure(2);hold on;
set(gca, 'FontSize', fontsize);
set(gca, 'LineWidth', linewidth);
plot(a01(:, 1), a01(:, 2), 'k:');
plot(a21(:, 1), a21(:, 2), 'k-');
plot(a(:, 1), a(:, 2), 'k-', 'linewidth', 3);
legend({'Whole Checkpointing', 'Min Max', 'No Checkpointing'}, 0)
xlabel('time (sec)'); 
ylabel('end-to-end latency (sec)'); 
axis([60 80 0 3]);


