vec3 = {}
vec3_mt = {__index = vec3}

function vec3:new(args)
    return setmetatable(args, vec3_mt)
end

function vec3:outer(rhs)
    local r = table.createtable(3,0)
    for i=1,3 do
        r[i] = table.createtable(3,0)
        
        local r_i = r[i]
        local self_i = self[i]
        
        for j=1,3 do
            r_i[j] = self_i * rhs[j]
        end
    end
    return mat3:new(r)
end

