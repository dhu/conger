function p = smooth(output, g, start_time, threshold)
    p = output(find(output(:, 1) == g), [4 5]);
    p(:, 1) = p(:, 1) - start_time;
    p = p/1000;
    if (threshold >= 0)
        i = 2;
        count = 0;
        indices = p(:,1);
        deleted = 0;
        while (i < length(p(:,1)))
            if (deleted == 0)
                before = p(i-1,2);
            end
            current = p(i, 2);
            after = p(i+1,2);
            min_y = min(before, after)-threshold;
            max_y = max(before, after)+threshold;
            if (min_y <= current && current <= max_y)
                deleted = 1;
    %            p = [p(1:i-1,:); p(i+1:end, :)];
            else
                indices(count+1) = i;
                count = count + 1;
    %            i = i+1;
            end
            i = i+1;
        end
        indices = indices(1:count);
        p = p(indices, :);
        whos p
    end
end