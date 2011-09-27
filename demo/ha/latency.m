close all; clear all;

start_time = 0; 
end_time = start_time+120000; 

%======================
load output.200.dat;

min_x = output(1, 3);
a01 = output(find(output(:, 1) == 1111), [3 4]);
a01(:, 2) = a01(:, 2) - a01(:, 1);
a01(:, 1) = a01(:, 1) - min_x;

a02 = output(find(output(:, 1) == 2111), [3 4]);
a02(:, 2) = a02(:, 2) - a02(:, 1);
a02(:, 1) = a02(:, 1) - min_x;

a03 = output(find(output(:, 1) == 3111), [3 4]);
a03(:, 2) = a03(:, 2) - a03(:, 1);
a03(:, 1) = a03(:, 1) - min_x;
%=========================
load server1.recovery_time.200.dat
load server2.recovery_time.200.dat
load server3.recovery_time.200.dat

s01 = server1;
s02 = server2;
s03 = server2;

s01(:, 1) = s01(:, 1) - min_x; s02(:, 1) = s02(:, 1) - min_x; s03(:, 1) = s03(:, 1) - min_x;
%=========================
a = s01;
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
s01 = a;

a = s02;
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
s02 = a;

a = s03;
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
s03 = a;
%=========================
a = a01;
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
a01 = a;

a = a02;
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
a02 = a;

a = a03;
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
a03 = a;
%=========================


%======================
load output.201.dat;

min_x = output(1, 3);
a11 = output(find(output(:, 1) == 1111), [3 4]);
a11(:, 2) = a11(:, 2) - a11(:, 1);
a11(:, 1) = a11(:, 1) - min_x;

a12 = output(find(output(:, 1) == 2111), [3 4]);
a12(:, 2) = a12(:, 2) - a12(:, 1);
a12(:, 1) = a12(:, 1) - min_x;

a13 = output(find(output(:, 1) == 3111), [3 4]);
a13(:, 2) = a13(:, 2) - a13(:, 1);
a13(:, 1) = a13(:, 1) - min_x;
%=========================
load server1.recovery_time.201.dat
load server2.recovery_time.201.dat
load server3.recovery_time.201.dat

s11 = server1;
s12 = server2;
s13 = server3;

s11(:, 1) = s11(:, 1) - min_x; s12(:, 1) = s12(:, 1) - min_x; s13(:, 1) = s13(:, 1) - min_x;
%=========================
a = s11;
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
s11 = a;

a = s12;
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
s12 = a;

a = s13;
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
s13 = a;
%=========================
a = a11;
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
a11 = a;

a = a12;
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
a12 = a;

a = a13;
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
a13 = a;
%=========================



%======================
load output.201.dat;

min_x = output(1, 3);
a21 = output(find(output(:, 1) == 1111), [3 4]);
a21(:, 2) = a21(:, 2) - a21(:, 1);
a21(:, 1) = a21(:, 1) - min_x;

a22 = output(find(output(:, 1) == 2111), [3 4]);
a22(:, 2) = a22(:, 2) - a22(:, 1);
a22(:, 1) = a22(:, 1) - min_x;

a23 = output(find(output(:, 1) == 3111), [3 4]);
a23(:, 2) = a23(:, 2) - a23(:, 1);
a23(:, 1) = a23(:, 1) - min_x;
%=========================
load server1.recovery_time.201.dat
load server2.recovery_time.201.dat
load server3.recovery_time.201.dat

s21 = server1;
s22 = server2;
s23 = server3;

s21(:, 1) = s21(:, 1) - min_x; s22(:, 1) = s22(:, 1) - min_x; s23(:, 1) = s23(:, 1) - min_x;
%=========================
a = s21;
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
s21 = a;

a = s22;
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
s22 = a;

a = s23;
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
s23 = a;
%=========================
a = a21;
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
a21 = a;

a = a22;
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
a22 = a;

a = a23;
i = find((a(:, 1) >= start_time) & (a(:, 1) <= end_time)); a = a(i, :);a(:, 1) = (a(:, 1) - start_time);a = a/1000;
a23 = a;
%=========================



%=========================
figure(1);hold on;
plot(s01(:, 1), s01(:, 2), 'k-');
plot(s02(:, 1), s02(:, 2), 'k:');
plot(s11(:, 1), s11(:, 2), 'b-');
plot(s12(:, 1), s12(:, 2), 'r:');
%plot(s21(:, 1), s21(:, 2), 'k-', 'linewidth', 3);
axis([0 120 0 15]);
ylabel('recovery time (sec)');

figure(2);hold on;
plot(a01(:, 1), a01(:, 2), 'k-');
plot(a02(:, 1), a02(:, 2), 'k:');
plot(a11(:, 1), a11(:, 2), 'b-');
plot(a12(:, 1), a12(:, 2), 'r:');
%plot(a21(:, 1), a21(:, 2), 'k-', 'linewidth', 3);
axis([0 120 0 5]);
ylabel('latency (sec)');
%=========================
