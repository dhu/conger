function values = load_logs(file_names, c1, c2)
values = zeros(length(file_names), 2);
for i = 1:length(file_names)
    file_name = char(file_names(i));
    l = load(file_name);
    len = length(l(:, 1));
    indices = ceil(len/7):floor(len*6/7);
    l = [l(indices, c1) l(indices, c2)];
    values(i, :) = mean(l, 1);
end
