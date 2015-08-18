mat3 = {}
mat3_mt = { __index = mat3 }

function mat3:new(args)
    return setmetatable(args, mat3_mt)
end

function mat3:eye()
    return mat3:new{{1,0,0}, {0,1,0}, {0,0,1}}
end

mat3_mt.__mul = function (lhs, rhs)
    if type(rhs) == "number" then
        return mat3:new(mat_scale(3,3,lhs,rhs))
    else
        assert(false, "unsupported multiplication type")
    end
end

mat3_mt.__add = function (lhs, rhs)
    return mat3:new(mat_add(3,3,lhs, rhs))
end

mat3_mt.__sub = function (lhs, rhs)
    return mat3:new(mat_sub(3,3,lhs,rhs))
end
