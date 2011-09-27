clear all; close all;

A11 = [216 1575; 857 1774];
%Y11 = [5.59; 10.0];
Y11 = [5.90; 13.56];
C11 = inv(A11)*Y11;

A12 = [430 1746; 1693 1517];
%Y12 = [7.35; 14.93];
Y12 = [9.19; 20.42];
C12 = inv(A12)*Y12;

A21 = [204 1764; 817 3122];
%Y21 = [5.60; 12.56];
Y21 = [5.52; 13.32];
C21 = inv(A21)*Y21;

A22 = [407 3496; 1633 2330];
%Y22 = [10.82; 15.82];
Y22 = [10.91; 18.82];
C22 = inv(A22)*Y22;

C = 0.25*(C11+C12+C21+C22);
A11*C;
A12*C;
A21*C;
A22*C;

C * 4 * 1000;

%%%%%%%%%%%%%%%%%%%%%%%%


N = 2;

%%%%%%%%%%%%%%%%%%%%%%%%

l1 = load('archive/input.125.log');
l1 = l1(end-1:end, :);
l2 = load('archive/input.250.log');
l2 = l2(end-1:end, :);
l3 = load('archive/input.500.log');
l3 = l3(end-1:end, :);
l4 = load('archive/input.1000.log');
l4 = l4(end-1:end, :);
l5 = load('archive/input.1500.log');
l5 = l5(end-1:end, :);
l6 = load('archive/input.2000.log');
l6 = l6(end-1:end, :);
l7 = load('archive/input.3000.log');
l7 = l7(end-1:end, :);
l8 = load('archive/input.4000.log');
l8 = l8(end-1:end, :);
I1 = [l1(1, :); l2(1, :); l3(1, :); l4(1, :); l5(1, :); l6(1, :); l7(1, :); l8(1, :)];
I2 = [l1(2, :); l2(2, :); l3(2, :); l4(2, :); l5(2, :); l6(2, :); l7(2, :); l8(2, :)];
I1
I2

I = [125 250 500 1000 1500 2000 3000 4000];
I = 0.001*I';
I = [I I1(:, 4) I2(:, 4) I1(:, 5:8) I2(:, 5:8)];
for i = 1:length(I(:, 1))
    I(i, 4:11) = I(i, 4:11) * I(i, 1);
end
I
J = zeros(4, length(I(:, 1)), 2);
J(1, :, 1) = I(:, 1);
J(1, :, 2) = I(:, 4);
J(2, :, 1) = I(:, 1);
J(2, :, 2) = I(:, 5);
J(3, :, 1) = I(:, 1);
J(3, :, 2) = I(:, 8);
J(4, :, 1) = I(:, 1);
J(4, :, 2) = I(:, 9);
J(:, :, 2) = J(:, :, 2)/4*1000;

J2 = zeros(4, length(I(:, 1)), 2);
J2(1, :, 1) = I(:, 1);
J2(1, :, 2) = I(:, 6);
J2(2, :, 1) = I(:, 1);
J2(2, :, 2) = I(:, 7);
J2(3, :, 1) = I(:, 1);
J2(3, :, 2) = I(:, 10);
J2(4, :, 1) = I(:, 1);
J2(4, :, 2) = I(:, 11);
J2(:, :, 2) = J2(:, :, 2)/4*1000;

J(1, :, 1)
J(:, :, 2)
J2(:, :, 2)

J3 = J2;
J3(:, :, 2) = abs(J2(:, :, 2) - J(:, :, 2));
J3;

for i = 1:length(J3(:, 1, 1))
    for j = 1:length(J3(1, :, 1))
        J3(i, j, 2) = J3(i, j, 2)/J(i, j, 2);
    end
end
J4 = mean(J3, 2);
J4(:, 2)

JJ = [J(1, :, 1); J(1, :, 2); J2(1, :, 2); J(2, :, 2); J2(2, :, 2); J(3, :, 2); J2(3, :, 2); J(4, :, 2); J2(4, :, 2)];
JJ = [JJ (JJ(:, 1)-JJ(:, 1))];

for i = 1:length(J4(:, 1, 2))
    j = J4(i, 1, 2)
    JJ(2*i+1, end) = j;
end
JJ = JJ*100;
JJ = round(JJ)*0.01;
JJ 




figure(1); hold on;
group_names = {
'aggr(input 1, primary)'
'aggr(input 1, backup)'
'aggr(input 2, primary)'
'aggr(input 2, backup)'
};
symbols = {'k-', 'k-', 'k--', 'k--'};
title = '';
x_label = 'checkpoint interval (sec)';
y_label = 'checkpoint cost (ms)';
graph(group_names, J, title, x_label, y_label, symbols, [1 3]);
legend(group_names, 0);

max_y = 21;
axis([0 4 0 max_y]);
plot([1 1], [0  max_y], 'k:');
%plot(c(:, 1), 1000*s, ':');

