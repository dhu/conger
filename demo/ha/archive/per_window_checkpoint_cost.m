load captures.log
i = find(captures(:, 3) > 5);
captures = captures(i, :);
X1 = captures(:, 1:2); y1 = captures(:, 3);
[b1, bint1] = regress(y1, X1, 0.05)
e1 = (X1*b1 - y1)./y1;
e1 = mean(abs(e1))

load pastes.log
i = find(pastes(:, 3) > 5);
pastes = pastes(i, :);
X2 = pastes(:, 1:2); y2 = pastes(:, 3);
[b2, bint2] = regress(y2, X2, 0.05)
e2 = (X2*b2 - y2)./y2;
e2 = mean(abs(e2))

1000*[b1' b2']








