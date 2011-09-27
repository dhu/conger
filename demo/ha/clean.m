function p = clean(p)
    indices = find(p(:, 2) <= 1);
    while (length(indices) > 0)
        i = indices(1);
        p = [p(1:i-1,:); p(i+1:end, :)];
        indices = find(p(:, 2) <= 1);
    end
end

