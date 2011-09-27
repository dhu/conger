function a = graph(group_names, values, t, x_label, y_label, symbols, indices)
% graph(group_names, values, t, x_label, y_label)
%  group_names: a 1xm cell arrya
%  values: mxnx2 array, where m is the number of groups, n is the number of x-y value pairs per group.
%  t: the title of the graph
%  x_label: the x-label of the graph
%  y_label: the y-label of the graph
%
% Author: Jeong-Hyon Hwang {jhhwang@cs.brown.edu}
% Date  : 2003/11/7

markersize = 12;
fontsize = 14;
linewidth = 1;

set(gca, 'FontSize', fontsize);
set(gca, 'LineWidth', linewidth);

x_values = zeros(length(group_names), length(values(1, :, 1)));
y_values = x_values;

for i = 1:length(x_values(:, 1))
  x_values(i, :) = values(i, :, 1);
  y_values(i, :) = values(i, :, 2);
end

for i = 1:length(x_values(:, 1))
  symbol = cell2mat(symbols(i));
   thick_line = 0;
   for k = 1:length(indices)
      index = indices(k);
      if (index == i)
         thick_line = 1;
      end
   end

   if (thick_line == 0)
      plot([x_values(i, 1)], [y_values(i, 1)], symbol, 'markersize', markersize, 'linewidth', linewidth);
   else
      plot([x_values(i, 1)], [y_values(i, 1)], symbol, 'markersize', markersize, 'linewidth', 3*linewidth);
   end
end

for i = 1:length(x_values(:, 1))
  symbol = cell2mat(symbols(i));
   thick_line = 0;
   for k = 1:length(indices)
      index = indices(k);
      if (index == i)
         thick_line = 1;
      end
   end
   if (thick_line == 0)
      plot([x_values(i, :)], [y_values(i, :)], symbol, 'markersize', markersize, 'linewidth', linewidth);
   else
      plot([x_values(i, :)], [y_values(i, :)], symbol, 'markersize', markersize, 'linewidth', 3*linewidth);
   end
end

legend(group_names, 0);
title(t);
xlabel(x_label); 
ylabel(y_label); 
