function a = area(x_vals, y_vals)
% area(x_vals, y_vals)
%
% Author: Jeong-Hyon Hwang {jhhwang@cs.brown.edu}
% Date  : 2003/11/7

a = 0;
for i = 1:(length(x_vals)-1)
   x1 = x_vals(i); x2 = x_vals(i+1);
   y1 = y_vals(i); y2 = y_vals(i+1);
   a = a + abs(x2 - x1)*(y1+y2)/2;
end
