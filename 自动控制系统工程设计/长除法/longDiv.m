function res = longDiv(num, den, bit)
    if nargin < 3
        bit = length(den) * 2;
    end
    if length(den) < length(num)
        disp('error z transform');
        return;
    end
    if length(den) ~= length(num)
       num = [zeros(1, length(den) - length(num))   , num]; 
    end
 
    res = [];
    m = num;
    for i = 1 : bit
        tempRes = m(1)/den(1);
        m = m - tempRes * den;
        m = [m(2:length(m)), 0];
        res = [res tempRes];
    end
