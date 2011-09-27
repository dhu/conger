close all; clear all;

%======================
load output.200.dat; % active standby
start_time = output(1, 3);

load server1.recovery_time.200.dat;
load server2.recovery_time.200.dat;
load server3.recovery_time.200.dat;
load server4.recovery_time.200.dat;
load server5.recovery_time.200.dat;

s1 = server1;
s2 = server2;
s3 = server3;
s4 = server4;
s5 = server5;

s1(:, 1) = s1(:, 1) - start_time; 
s1 = s1/1000;

s2(:, 1) = s2(:, 1) - start_time; 
s2 = s2/1000;

s3(:, 1) = s3(:, 1) - start_time; 
s3 = s3/1000;

s4(:, 1) = s4(:, 1) - start_time; 
s4 = s4/1000;

s5(:, 1) = s5(:, 1) - start_time; 
s5 = s5/1000;

%======================
load output.300.dat;  % no checkpointing

start_time = output(1, 3);
a5 = smooth(output, 12011, start_time, -1);
%a5 = smooth(output, 52011, start_time, 0);

%======================
load output.301.dat;  % whole checkpointing

start_time = output(1, 3);

a03 = smooth(output, 32011, start_time, -1);
%a03 = smooth(output, 32011, start_time, 0.01);

load server1.recovery_time.301.dat;
load server2.recovery_time.301.dat;
load server3.recovery_time.301.dat;
load server4.recovery_time.301.dat;
load server5.recovery_time.301.dat;

s01 = server1;
s02 = server2;
s03 = server3;
s04 = server4;
s05 = server5;

s01(:, 1) = s01(:, 1) - start_time; 
s01 = s01/1000;

s02(:, 1) = s02(:, 1) - start_time; 
s02 = s02/1000;

s03(:, 1) = s03(:, 1) - start_time;
s03 = s03/1000;

s04(:, 1) = s04(:, 1) - start_time;
s04 = s04/1000;

s05(:, 1) = s05(:, 1) - start_time;
s05 = s05/1000;

%======================
load output.302.dat; % round robin

start_time = output(1, 3);

a13 = smooth(output, 32011, start_time, -1);
%a13 = smooth(output, 32011, start_time, 0);

%=========================
load server1.recovery_time.302.dat;
load server2.recovery_time.302.dat;
load server3.recovery_time.302.dat;
load server4.recovery_time.302.dat;
load server5.recovery_time.302.dat;

s11 = server1;
s12 = server2;
s13 = server3;
s14 = server4;
s15 = server5;

s11(:, 1) = s11(:, 1) - start_time; 
s11 = s11/1000;

s12(:, 1) = s12(:, 1) - start_time; 
s12 = s12/1000;

s13(:, 1) = s13(:, 1) - start_time;
s13 = s13/1000;

s14(:, 1) = s14(:, 1) - start_time;
s14 = s14/1000;

s15(:, 1) = s15(:, 1) - start_time;
s15 = s15/1000;


fontsize = 14;
linewidth = 1;

%======================
load output.303.dat; % min_max

start_time = output(1, 3);

a23 = smooth(output, 32011, start_time, -1);
%a23 = smooth(output, 32011, start_time, 0);

%=========================
load server1.recovery_time.303.dat;
load server2.recovery_time.303.dat;
load server3.recovery_time.303.dat;
load server4.recovery_time.303.dat;
load server5.recovery_time.303.dat;

s21 = server1;
s22 = server2;
s23 = server3;
s24 = server4;
s25 = server5;

s21(:, 1) = s21(:, 1) - start_time; 
s21 = s21/1000;

s22(:, 1) = s22(:, 1) - start_time; 
s22 = s22/1000;

s23(:, 1) = s23(:, 1) - start_time;
s23 = s23/1000;

s24(:, 1) = s24(:, 1) - start_time;
s24 = s24/1000;

s25(:, 1) = s25(:, 1) - start_time;
s25 = s25/1000;


fontsize = 14;
linewidth = 1;

%=========================
figure(1);hold on;
group_names = {
'active standby'
'whole checkpointing (passive standby)'
'round robin'
'min max'
};
set(gca, 'FontSize', fontsize);
set(gca, 'LineWidth', linewidth);

plot(s5(:, 1), s5(:, 2), 'g-', 'linewidth', 3); % active standby
plot(s01(:, 1), s01(:, 2)*3, 'k:'); % because didn't yet consider paste cost.
plot(s11(:, 1), s11(:, 2), 'k-.', 'linewidth', 1);
plot(s21(:, 1), s21(:, 2), 'k-', 'linewidth', 3);

plot(s12(:, 1), s12(:, 2), 'k-.', 'linewidth', 2);
plot(s13(:, 1), s13(:, 2), 'k-.', 'linewidth', 2);
plot(s14(:, 1), s14(:, 2), 'k-.', 'linewidth', 2);
plot(s15(:, 1), s15(:, 2), 'k-.', 'linewidth', 2);

plot(s22(:, 1), s22(:, 2), 'k-', 'linewidth', 3);
plot(s23(:, 1), s23(:, 2), 'k-', 'linewidth', 3);
plot(s24(:, 1), s24(:, 2), 'k-', 'linewidth', 3);
plot(s25(:, 1), s25(:, 2), 'k-', 'linewidth', 3);


legend(group_names, 0);
axis([0 200 0 15]);
ylabel('recovery time (sec)');
xlabel('time (sec)');

%=========================

group_names = {
'no HA / active standby'
'whole checkpointing (passive standby)'
'min max / round robin'
};

figure(2);hold on;
set(gca, 'FontSize', fontsize);
set(gca, 'LineWidth', linewidth);

plot(a5(:, 1), a5(:, 2), 'g-', 'linewidth', 2);
plot(a03(:, 1), a03(:, 2), 'k:');
plot(a13(:, 1), a13(:, 2), 'k-');
plot(a5(:, 1), a5(:, 2), 'g-', 'linewidth', 2);

legend(group_names, 'Location', 'SouthEast');
axis([0 200 0 2]);
ylabel('latency (sec)');
xlabel('time (sec)');
%=========================
