function [c, l] = sieve(input, g)
    c = input(find(input(:,2) == g), [1 6 7 8]);
    l = mean(input(find(input(:,2) == g), 5));
    times = c(:, 1);
    times = times - [times(1); times(1:end-1)];
    c(:, 1) = times*0.001;
    c = c(2:end, :);
    c = sortrows(c, 1);
end