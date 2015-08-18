--- Matrix addition.
function mat_add(n, m, lhs, rhs)
    local out = table.createtable(n, 0)
    for i=1,n do
        out[i] = table.createtable(m, 0)
        
        local out_i = out[i]
        local lhs_i = lhs[i]
        local rhs_i = rhs[i]
        
        for j=1,n do
            out_i[j] = lhs_i[j] + rhs_i[j]
        end
    end
    return out
end

--- Matrix subtraction.
function mat_sub(n, m, lhs, rhs)
    local out = table.createtable(n, 0)
    for i=1,n do
        out[i] = table.createtable(m, 0)
        
        local out_i = out[i]
        local lhs_i = lhs[i]
        local rhs_i = rhs[i]
        
        for j=1,n do
            out_i[j] = lhs_i[j] - rhs_i[j]
        end
    end
    return out
end

--- Matrix scaling.
function mat_scale(n, m, lhs, rhs)
    local out = table.createtable(n, 0)
    for i=1,n do
        out[i] = table.createtable(m, 0)
        
        local out_i = out[i]
        local lhs_i = lhs[i]
        
        for j=1,n do
            out_i[j] = lhs_i[j] * rhs
        end
    end
    return out
end

--- Matrix multiplication.
-- @param lhs nxm matrix
-- @param rhs mxp matrix
-- @return nxp matrix
function mat_mul(n,m,p,lhs,rhs)
    local out = table.createtable(n, 0)
    for i=1,n do
        out[i] = table.createtable(p, 0)
        
        local out_i = out[i]
        local lhs_i = lhs[i]
        
        for j=1,p do
            local sum = 0
            
            for k=1,m do
                sum = sum + lhs_i[k] * rhs[k][j]
            end
            
            out_i[j] = sum
        end
    end
    return out
end
