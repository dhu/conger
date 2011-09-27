function y2 = appr(x, y)
X = log(x);
Y = log(y);
m_X = mean(X);
m_Y = mean(Y);
sum1 = 0;
sum2 = 0;
for i = 1:length(X)
    sum1 = sum1 + (X(i)-m_X)*(Y(i)-m_Y);
    sum2 = sum2 + (X(i)-m_X)*(X(i)-m_X);
end
b = sum1/sum2;
a = exp(mean(Y) - b * mean(X));
[a b]
y2 = y;
for i = 1:length(x)
  y2(i) = a * x(i)^b ;
end
y2;
