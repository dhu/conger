function c = condense(c)
    max_time = max(c(:,1));
    for i = 0.1:0.1:max_time
        indices = find(c(:,1) >= i-0.1 & c(:,1) < i);
        while(length(indices) > 1)
            j = indices(end);
            c = [c(1:j-1, :); c(j+1:end, :)];
            indices = find(c(:,1) >= i-0.1 & c(:,1) < i);
        end
    end
end

